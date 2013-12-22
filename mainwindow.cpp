#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "savedialog.hpp"

#include "drawutils.hpp"
#include "qtopencv.hpp"

#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QMutexLocker>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#define PROCESS_TIMEOUT 20

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _cap(0),
  _sudokuFinder(40),
  _digitExtractor(_sudokuFinder)
{
  ui->setupUi(this);

  connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveCells()));

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
      cv::Mat cell;
      if (_digitExtractor.cell(row, col, cell))
      {
        _sudokuCells[row][col] = QtOpenCV::MatToQImage(cell, QImage::Format_Indexed8);
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
