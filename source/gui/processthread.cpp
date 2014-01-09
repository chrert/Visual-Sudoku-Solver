#include "../../include/gui/processthread.hpp"

#include "../../include/gui/mainwindow.hpp"
#include "../../include/settings.hpp"
#include "../../include/utils/qtopencv.hpp"
#include "../../include/utils/drawutils.hpp"
#include "../../include/classification/knndigitclassifier.hpp"
#include "../../include/classification/svmdigitclassifier.hpp"
#include "../../include/classification/nndigitclassifier.hpp"

#include <QMutexLocker>
#include <QTextBlock>
#include <QTextCursor>

ProcessThread::ProcessThread(MainWindow *window) :
  _mainWindow(window),
  _sudokuFinder(SUDOKU_CELL_WORKING_SIZE),
  _digitExtractor(_sudokuFinder),
  _running(true),
  _classify(false),
  _videoCapture(CAM_NUM),
  _extractorFinderMutex(),
  _classifierMutex()
{
#ifdef USE_KNN_CLASSIFIER
    _digitClassifier = new KNNDigitClassifier(_digitExtractor, DIGIT_SAMPLE_WIDTH, KNN_K, PCA_COMPONENTS);
#elif defined USE_SVM_CLASSIFIER
    _digitClassifier = new SVMDigitClassifier(_digitExtractor, DIGIT_SAMPLE_WIDTH, PCA_COMPONENTS);
#else
    _digitClassifier = new NNDigitClassifier(_digitExtractor, DIGIT_SAMPLE_WIDTH, PCA_COMPONENTS);
#endif

    setupResponses();

    // needed to prevent QT warnings. No idea where they come from...
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");
}

ProcessThread::~ProcessThread()
{
  delete _digitClassifier;
}

bool ProcessThread::containsDigit(size_t row, size_t col)
{
  QMutexLocker lock(&_extractorFinderMutex);
  return _digitExtractor.containsDigit(row, col);
}

QImage ProcessThread::getDigitCell(size_t row, size_t col)
{
  QMutexLocker lock(&_extractorFinderMutex);

  cv::Mat cell;
  if (! _sudokuFinder.cell(row, col, cell))
    return QImage(_sudokuFinder.getCellSize(), _sudokuFinder.getCellSize(), QImage::Format_RGB888);

  return QtOpenCV::MatToQImage(cell, QImage::Format_RGB888);
}

QImage ProcessThread::getPreparedDigitCell(size_t row, size_t col)
{
  QMutexLocker lock(&_extractorFinderMutex);

  cv::Mat cell;
  if (! _digitExtractor.cell(row, col, cell))
    return QImage(_sudokuFinder.getCellSize(), _sudokuFinder.getCellSize(), QImage::Format_Indexed8);

  return QtOpenCV::MatToQImage(cell, QImage::Format_Indexed8);
}

uchar ProcessThread::getDigit(size_t row, size_t col)
{
  QMutexLocker lock1(&_extractorFinderMutex);

  cv::Mat cell;
  bool foundCell = _sudokuFinder.cell(row, col, cell);

  if (! _classify || ! foundCell)
    return NO_DIGIT_FOUND;

  QMutexLocker lock2(&_classifierMutex);
  return _digitClassifier->classify(cell);
}

void ProcessThread::train(const std::vector<cv::Mat> *trainingImages)
{
  QMutexLocker lock1(&_extractorFinderMutex);
  QMutexLocker lock2(&_classifierMutex);

  _digitClassifier->train(trainingImages);
  _classify = true;
}

bool ProcessThread::loadClassifier(const QString &filename)
{
  QMutexLocker lock(&_classifierMutex);
  return (_classify = _digitClassifier->load(qPrintable(filename)));
}

bool ProcessThread::saveClassifier(const QString &filename)
{
  QMutexLocker lock(&_classifierMutex);
  return _digitClassifier->save(qPrintable(filename));
}

void ProcessThread::stop()
{
  _mainWindow->printOnConsole("Stopping...");
  _running = false;
}

void ProcessThread::setupResponses()
{
  _responseCount = 0;
  _allFixed = false;
  _fixedSent = false;
  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
  {
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
    {
      _digitFixed[row][col] = false;
      for (size_t i = 0; i < NUM_FRAMES_FIXED; ++i)
        _digitResponses[row][col][i] = i;
    }
  }
}

void ProcessThread::run()
{
  if (! _videoCapture.isOpened())
  {
    _mainWindow->printOnConsole("Could not open cam!");
    return;
  }

  _mainWindow->printOnConsole("Application running...");

  _found = false;
  _lostCount = 0;

  while (_running)
  {
    if (! _videoCapture.grab())
    {
      _mainWindow->printOnConsole("No picture from cam! Stop capturing...");
      return;
    }

    cv::Mat frame, frameRGB;
    _videoCapture >> frame;

    QMutexLocker extractorLock(&_extractorFinderMutex);

    if (_sudokuFinder.updateFrame(frame))
    {
      _lostCount = 0;
      if (! _found)
      {
        _found = true;
        emit sudokuAppeared();
      }

      const Color &frameColor = _allFixed ? DrawUtils::COLOR_GREEN : DrawUtils::COLOR_RED;
      DrawUtils::drawContour(frame, _sudokuFinder.getFoundSudokuContour(), frameColor, 3);

      _digitExtractor.updateCells();

      if (_classify)
      {
        QMutexLocker classifierLock(&_classifierMutex);
        classifyDigits();
      }
    }
    else if (++_lostCount == NUM_FRAMES_LOST)
    {
      _found = false;
      _lostCount = 0;
      setupResponses();
      emit sudokuDisappeared();
    }

    cv::cvtColor(frame, frameRGB, CV_BGR2RGB);
    _frame = QtOpenCV::MatToQImage(frameRGB, QImage::Format_RGB888);
    emit newFrame(&_frame);

    msleep(20);
  }
}

void ProcessThread::classifyDigits()
{
  _allFixed = true;
  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
  {
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
    {
      if (_digitFixed[row][col])
        continue;

      uchar digit = NO_DIGIT_FOUND;

      cv::Mat cell;
      if (_sudokuFinder.cell(row, col, cell) && _digitExtractor.containsDigit(row, col))
        digit = _digitClassifier->classify(cell);

      emit digitChanged(row, col, digit);

      _digitResponses[row][col][_responseCount] = digit;
      bool fix = true;
      for (size_t i = 0; i < NUM_FRAMES_FIXED && fix; ++i)
        if (digit != _digitResponses[row][col][i])
          fix = false;
      if (fix)
      {
        _digitFixed[row][col] = true;
        emit digitFixed(row, col, digit);
      }
      else
      {
        _allFixed = false;
        _fixedSent = false;
      }
    }
  }

  if (++_responseCount == NUM_FRAMES_FIXED)
    _responseCount = 0;

  if (_allFixed && ! _fixedSent)
  {
    _fixedSent = true;
    emit allDigitsFixed();
  }
}
