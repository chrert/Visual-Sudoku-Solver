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

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _consoleLock()
{
  ui->setupUi(this);

  connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveCells()));
  connect(ui->actionTrain, SIGNAL(triggered()), this, SLOT(trainClassifier()));
//  connect(ui->actionCrossValidation, SIGNAL(triggered()), this, SLOT(crossValidation()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveClassifier()));
  connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadClassifier()));

  setupSudokuGrid();
  this->adjustSize();

  _processThread = new ProcessThread(this);
  qRegisterMetaType<size_t>("size_t");
  connect(_processThread, SIGNAL(newFrame(const QImage*)), this, SLOT(updateCamView(const QImage*)));
  connect(_processThread, SIGNAL(digitChanged(size_t,size_t,uchar)), this, SLOT(updateSudokuView(size_t,size_t,uchar)));
  connect(_processThread, SIGNAL(digitFixed(size_t,size_t,uchar)), this, SLOT(fixSudokuView(size_t,size_t,uchar)));
  connect(_processThread, SIGNAL(sudokuDisappeared()), this, SLOT(clearSudokuView()));
  _processThread->start();
}

MainWindow::~MainWindow()
{
  delete ui;
  delete _processThread;
}

void MainWindow::closing()
{
  _processThread->stop();
  _processThread->wait();
}

void MainWindow::updateCamView(const QImage *image)
{
  ui->camView->setPixmap(QPixmap::fromImage(*image));
}

void MainWindow::updateSudokuView(size_t row, size_t col, uchar response)
{
  _digitViews[row][col]->setPalette(Qt::red);
  _digitViews[row][col]->display(QString::number(response));
  _digitViews[row][col]->setEnabled(response != 0);
}

void MainWindow::fixSudokuView(size_t row, size_t col, uchar response)
{
  _digitViews[row][col]->setPalette(Qt::green);
}

void MainWindow::clearSudokuView()
{
  setupSudokuGrid();
}

void MainWindow::printOnConsole(const QString &msg)
{
  QMutexLocker locker(&_consoleLock);
  ui->console->appendPlainText(msg);
}

void MainWindow::setupSudokuGrid()
{
  for (size_t row = 0; row < 9; ++row)
  {
    for (size_t col = 0; col < 9; ++col)
    {
      QLCDNumber *lcd = _digitViews[row][col];
      if (lcd == nullptr)
        lcd = new QLCDNumber(1, this);
      lcd->display(QString::number(NO_DIGIT_FOUND));
      lcd->setPalette(Qt::black);
      lcd->setEnabled(false);
      lcd->adjustSize();
      _digitViews[row][col] = lcd;
      ui->sudokuGrid->addWidget(lcd, row, col);
    }
  }
}

void MainWindow::saveCells()
{
  printOnConsole("Start saving training data...");

  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
  {
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
    {
      if (_processThread->containsDigit(row, col))
      {
        QImage cellImg = _processThread->getDigitCell(row, col);
        SaveDialog* dlg = new SaveDialog();
        dlg->setImage(cellImg);
        dlg->exec();
        dlg->save();
        delete dlg;
      }
    }
  }

  printOnConsole("Saving training data done!");
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

  printOnConsole("Start training...");;
  _processThread->train(trainingImages);
  printOnConsole("Done training!");

  ui->actionSave->setEnabled(true);
}

//void MainWindow::crossValidation()
//{
//  std::vector<cv::Mat> samples[9];
//  if (! askForTrainingSamples(samples))
//    return;

//  size_t k = 9;

//  std::stringstream ss;
//  ss << "Splitting the samples in k=" << k << " parts";
//  printOnConsole(ss.str().c_str());

//  float meanHitRate = 0.f;
//  for (size_t i = 0; i < k; ++i)
//  {
//    ss.str("");
//    ss << "Test with fold i=" << i << " and train with others";
//    printOnConsole(ss.str().c_str());

//    std::vector<cv::Mat> testSet[9];
//    for (size_t j = 0; j < 9; ++j)
//    {
//      size_t numSamples = samples[j].size();
//      size_t foldSize = numSamples / k;

//      auto testStart = std::begin(samples[j]) + (i * foldSize);
//      auto testEnd = testStart + foldSize;
//      if (i == (k-1))
//        testEnd = std::end(samples[j]);

//      if (testStart == std::end(samples[j]))
//        return;

//      for (auto it = testStart; it != testEnd; ++it)
//        testSet[j].push_back(*it);

//      samples[j].erase(testStart, testEnd);
//    }

//    _digitClassifier->train(samples);

//    size_t hits = 0;
//    size_t testSetSize = 0;
//    for (size_t j = 0; j < 9; ++j)
//    {
//      testSetSize += testSet[j].size();
//      for (auto it = std::begin(testSet[j]); it != std::end(testSet[j]); ++it)
//      {
//        uchar c = _digitClassifier->classifiy(*it);
//        if (c == (j+1))
//          ++hits;
//        std::cout << "Real: " << j+1 << " / Got: " << (int)c << std::endl;
//      }
//    }

//    float hitRate = static_cast<float>(hits) / testSetSize;

//    ss.str("");
//    ss << "Hit-rate: " << hitRate;
//    printOnConsole(ss.str().c_str());

//    meanHitRate += 1.f/k * hitRate;
//  }

//  printOnConsole("Cross validation done!");
//  ss.str("");
//  ss << "Mean hit-rate: " << meanHitRate;
//  printOnConsole(ss.str().c_str());
//}

void MainWindow::saveClassifier()
{
  const QString filename = QFileDialog::getSaveFileName(this, tr("Where to save?"));

  if (_processThread->saveClassifier(filename))
    printOnConsole("Saved classifier...");
  else
    printOnConsole("Cannot save classifier!");
}

void MainWindow::loadClassifier()
{
  ui->actionSave->setEnabled(false);

  const QString filename = QFileDialog::getOpenFileName(this, tr("Choose classifier file"));

  if (_processThread->loadClassifier(filename))
    printOnConsole("Loaded classifier...");
  else
    printOnConsole("Cannot load classifier!");
}
