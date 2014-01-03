#include "ui_mainwindow.h"
#include "../../include/gui/mainwindow.hpp"
#include "../../include/gui/savedialog.hpp"

#include "../../include/utils/drawutils.hpp"
#include "../../include/utils/qtopencv.hpp"
#include "../../include/classification/svmdigitclassifier.hpp"
#include "../../include/classification/knndigitclassifier.hpp"
#include "../../include/classification/nndigitclassifier.hpp"

#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QMutexLocker>
#include <QFileDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

#define PROCESS_TIMEOUT 200

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _cap(0),
  _sudokuFinder(40),
  _digitExtractor(_sudokuFinder),
  _classify(false)
{
  _digitClassifier = new SVMDigitClassifier(_digitExtractor, 16);

  ui->setupUi(this);

  connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveCells()));
  connect(ui->actionTrain, SIGNAL(triggered()), this, SLOT(trainClassifier()));
  connect(ui->actionCrossValidation, SIGNAL(triggered()), this, SLOT(crossValidation()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveClassifier()));
  connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadClassifier()));

  if (! _cap.isOpened())
  {
    printOnConsole("Could not open cam!");
    return;
  }

  setupSudokuGrid();

  int width = static_cast<int>(_cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  int height = static_cast<int>(_cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  ui->camView->resize(width, height);

  this->adjustSize();

  _processTimer = new QTimer(this);
  connect(_processTimer, SIGNAL(timeout()), this, SLOT(process()));

  printOnConsole("Application running...");
  _processTimer->start(PROCESS_TIMEOUT);
}

MainWindow::~MainWindow()
{
  delete ui;
  delete _digitClassifier;
}

void MainWindow::process()
{
  QMutexLocker lock(&_processMutex);

  if (! _cap.grab())
  {
    printOnConsole("No picture from cam! Stop capturing...");
    _processTimer->stop();
    return;
  }

  cv::Mat frame;
  _cap >> frame;

  if (_sudokuFinder.updateFrame(frame))
  {
    DrawUtils::drawContour(frame, _sudokuFinder.getFoundSudokuContour(), cv::Scalar(0, 255, 0));

    if (! _digitExtractor.updateCells())
      printOnConsole("Error while processing digits!");

  }

  updateSudokuView();
  updateCamView(frame);

}

void MainWindow::updateCamView(const cv::Mat &mat)
{
  cv::Mat tmp = mat.clone();
  cv::cvtColor(mat, tmp, CV_BGR2RGB);
  _qFrame = QtOpenCV::MatToQImage(tmp, QImage::Format_RGB888);
  ui->camView->setPixmap(QPixmap::fromImage(_qFrame));
}

void MainWindow::updateSudokuView()
{
  for (size_t row = 0; row < 9; ++row)
  {
    for (size_t col = 0; col < 9; ++col)
    {
      cv::Mat disp = cv::Mat::ones(40, 40, CV_8UC3);
      disp = cv::Scalar(255,255,255);
      cv::Mat cell;
      if (_digitExtractor.cell(row, col, cell))
      {
        if (_classify && _digitExtractor.containsDigit(row, col))
        {
          cv::Mat cell1;
          if (_sudokuFinder.cell(row, col, cell1))
          {
            uchar c = _digitClassifier->classifiy(cell1);
            std::stringstream ss;
            ss << static_cast<int>(c);
            cv::putText(disp, ss.str(), cv::Point(0,_sudokuFinder.getCellSize()), CV_FONT_HERSHEY_PLAIN, 3, cv::Scalar(255,0,0));
          }
        }
        _sudokuCells[row][col] = QtOpenCV::MatToQImage(disp, QImage::Format_RGB888);
        _sudokuViews[row][col]->setPixmap(QPixmap::fromImage(_sudokuCells[row][col]));
      }
    }
  }
}

void MainWindow::printOnConsole(const QString &msg)
{
  ui->console->appendPlainText(msg);
}

void MainWindow::setupSudokuGrid()
{
  size_t cellSize = _sudokuFinder.getCellSize();

  for (size_t row = 0; row < 9; ++row)
  {
    for (size_t col = 0; col < 9; ++col)
    {
      QLabel* label = new QLabel(this);
      label->resize(cellSize, cellSize);
      _sudokuViews[row][col] = label;
      ui->sudokuGrid->addWidget(label, row, col);
    }
  }
}

void MainWindow::saveCells()
{
  QMutexLocker lock(&_processMutex);
  _processTimer->stop();

  printOnConsole("Start saving training data...");

  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
  {
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
    {
      if (_digitExtractor.containsDigit(row, col))
      {
        cv::Mat cell;
        _sudokuFinder.cell(row, col, cell);
        QImage cellImg = QtOpenCV::MatToQImage(cell, QImage::Format_RGB888);
        SaveDialog* dlg = new SaveDialog();
        dlg->setImage(cellImg);
        dlg->exec();
        dlg->save();
        delete dlg;
      }
    }
  }

  printOnConsole("Saving training data done!");

  _processTimer->start(PROCESS_TIMEOUT);
}

bool MainWindow::askForTrainingSamples(std::vector<cv::Mat>* samples)
{
  printOnConsole("Choosing training data from file...");
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Choose parent directory"));

  QDir dir(dirName);
  if (! dir.exists())
  {
    printOnConsole("Directory does not exist!");
    return false;
  }

  printOnConsole("Reading training files...");
  QStringList nameFilter;
  nameFilter << "*.png";
  for (int i = 0; i < 9; ++i)
  {
    QDir digitDir(dir.absoluteFilePath(QString::number(i + 1)));

    if (! digitDir.exists())
    {
      printOnConsole("Error loading training images. Dir does not exist!");
      return false;
    }

    QFileInfoList files = digitDir.entryInfoList(nameFilter, QDir::Files);
    QFileInfoList::Iterator it = files.begin();
    for (; it != files.end(); ++it)
    {
      std::string path = qPrintable(it->absoluteFilePath());
      cv::Mat m = cv::imread(path);
      samples[i].push_back(m);
    }

    std::stringstream ss;
    ss << "Found " << files.size() << " training images for digit " << (i + 1);
    printOnConsole(ss.str().c_str());
  }
  printOnConsole("Done reading files...");
  return true;
}


void MainWindow::trainClassifier()
{
  std::vector<cv::Mat> trainingImages[9];
  if (! askForTrainingSamples(trainingImages))
    return;

  printOnConsole("Start training...");
  update();
  qApp->processEvents();
  _digitClassifier->train(trainingImages);
  printOnConsole("Done training!");

  _classify = true;

  ui->actionSave->setEnabled(true);
}

void MainWindow::crossValidation()
{
  std::vector<cv::Mat> samples[9];
  if (! askForTrainingSamples(samples))
    return;

  size_t k = 9;

  std::stringstream ss;
  ss << "Splitting the samples in k=" << k << " parts";
  printOnConsole(ss.str().c_str());

  float meanHitRate = 0.f;
  for (size_t i = 0; i < k; ++i)
  {
    ss.str("");
    ss << "Test with fold i=" << i << " and train with others";
    printOnConsole(ss.str().c_str());

    std::vector<cv::Mat> testSet[9];
    for (size_t j = 0; j < 9; ++j)
    {
      size_t numSamples = samples[j].size();
      size_t foldSize = numSamples / k;

      auto testStart = std::begin(samples[j]) + (i * foldSize);
      auto testEnd = testStart + foldSize;
      if (i == (k-1))
        testEnd = std::end(samples[j]);

      if (testStart == std::end(samples[j]))
        return;

      for (auto it = testStart; it != testEnd; ++it)
        testSet[j].push_back(*it);

      samples[j].erase(testStart, testEnd);
    }

    _digitClassifier->train(samples);

    size_t hits = 0;
    size_t testSetSize = 0;
    for (size_t j = 0; j < 9; ++j)
    {
      testSetSize += testSet[j].size();
      for (auto it = std::begin(testSet[j]); it != std::end(testSet[j]); ++it)
      {
        uchar c = _digitClassifier->classifiy(*it);
        if (c == (j+1))
          ++hits;
        std::cout << "Real: " << j+1 << " / Got: " << (int)c << std::endl;
      }
    }

    float hitRate = static_cast<float>(hits) / testSetSize;

    ss.str("");
    ss << "Hit-rate: " << hitRate;
    printOnConsole(ss.str().c_str());

    meanHitRate += 1.f/k * hitRate;
  }

  printOnConsole("Cross validation done!");
  ss.str("");
  ss << "Mean hit-rate: " << meanHitRate;
  printOnConsole(ss.str().c_str());
}

void MainWindow::saveClassifier()
{
  std::string filename = qPrintable(QFileDialog::getSaveFileName(this, tr("Where to save?")));

  if (_digitClassifier->save(filename))
    printOnConsole("Saved classifier...");
  else
    printOnConsole("Cannot save classifier!");
}

void MainWindow::loadClassifier()
{
  ui->actionSave->setEnabled(false);

  std::string filename = qPrintable(QFileDialog::getOpenFileName(this, tr("Choose classifier file")));

  if (_digitClassifier->load(filename))
  {
    printOnConsole("Loaded classifier...");
    _classify = true;
  }
  else
  {
    printOnConsole("Cannot load classifier!");
  }
}
