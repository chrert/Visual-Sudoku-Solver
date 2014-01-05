#ifndef PROCESSTHREAD_HPP
#define PROCESSTHREAD_HPP

#include <QThread>
#include <QMutex>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>

#include "../../include/gui/mainwindow.hpp"

#include "../../include/classification/digitclassifier.hpp"
#include "../../include/imgproc/digitextractor.hpp"
#include "../../include/imgproc/sudokufinder.hpp"

class MainWindow;

class ProcessThread : public QThread
{
  Q_OBJECT

public:
  ProcessThread(MainWindow *window);
  ~ProcessThread();

  bool containsDigit(size_t row, size_t col);

  QImage getDigitCell(size_t row, size_t col);
  QImage getPreparedDigitCell(size_t row, size_t col);

  uchar getDigit(size_t row, size_t col);

  void train(std::vector<cv::Mat> *trainingImages);
  bool loadClassifier(const QString &filename);
  bool saveClassifier(const QString &filename);

  void stop();

signals:
  void newFrame(const QImage *frame);

  void sudokuAppeared();
  void sudokuDisappeared();

  void digitChanged(size_t row, size_t col, uchar digit);
  void digitFixed(size_t row, size_t col, uchar digit);
  void allDigitsFixed();

private:
  MainWindow *_mainWindow;

  bool _running;
  bool _classify;

  QImage _frame;

  DigitClassifier *_digitClassifier;
  SudokuFinder    _sudokuFinder;
  DigitExtractor  _digitExtractor;

  cv::VideoCapture _videoCapture;

  QMutex _classifierMutex;
  QMutex _extractorFinderMutex;

  size_t _responseCount;
  uchar _digitResponses[NUM_ROWS_CELLS][NUM_ROWS_CELLS][NUM_FRAMES_FIXED];
  bool _digitFixed[NUM_ROWS_CELLS][NUM_ROWS_CELLS];
  bool _allFixed;

  bool _found;
  size_t _lostCount;

  void setupResponses();

  void run();

  void classifyDigits();
};

#endif // PROCESSTHREAD_HPP
