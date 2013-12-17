#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QImage>
#include <QTimer>
#include <QString>
#include <QLabel>
#include <opencv2/highgui/highgui.hpp>

#include "sudokufinder.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void process();

protected:
  void updateCamView(const cv::Mat& mat);
  void updateSudokuView();

  void printOnConsole(const QString& msg);

private:
  Ui::MainWindow *ui;

  cv::VideoCapture _cap;
  SudokuFinder _sudokuFinder;

  QTimer* processTimer;

  QImage _qFrame;

  QImage  _sudokuCells[9][9];
  QLabel* _sudokuViews[9][9];

  void setupSudokuGrid();
};

#endif // MAINWINDOW_HPP
