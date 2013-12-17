#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "drawutils.hpp"
#include "qtopencv.hpp"

#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _cap(0),
  _sudokuFinder(40)
{
  ui->setupUi(this);

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

  processTimer = new QTimer();
  connect(processTimer, SIGNAL(timeout()), this, SLOT(process()));

  printOnConsole("Application running...");
  processTimer->start(20);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::process()
{
  if (! _cap.grab())
  {
    printOnConsole("No picture from cam! Stop capturing...");
    processTimer->stop();
    return;
  }

  cv::Mat frame;
  _cap >> frame;

  size_t sudokuSize = _sudokuFinder.getRectificationSize();
  cv::Mat sudoku_mat = cv::Mat::zeros(sudokuSize, sudokuSize, CV_8UC1);

  if (_sudokuFinder.updateFrame(frame))
  {
    DrawUtils::drawContour(frame, _sudokuFinder.getFoundSudokuContour(), cv::Scalar(0, 255, 0));

    sudoku_mat = _sudokuFinder.getRectifiedSudoku().clone();
    cv::cvtColor(sudoku_mat, sudoku_mat, CV_BGR2GRAY);
    //cv::threshold(sudoku_mat, sudoku_mat, 100, 255, cv::THRESH_BINARY_INV);
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
      _sudokuFinder.cell(row, col, cell);
      _sudokuCells[row][col] = QtOpenCV::MatToQImage(cell, QImage::Format_RGB888);
      _sudokuViews[row][col]->setPixmap(QPixmap::fromImage(_sudokuCells[row][col]));
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
