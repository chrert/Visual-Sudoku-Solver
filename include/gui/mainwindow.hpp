#ifndef MAINWINDOW_HPP__
#define MAINWINDOW_HPP__

#include <QMainWindow>
#include <QImage>
#include <QTimer>
#include <QString>
#include <QLabel>
#include <QMutex>
#include <QLCDNumber>
#include <QThread>

#include <opencv2/highgui/highgui.hpp>

#include "processthread.hpp"
#include "../imgproc/sudokufinder.hpp"
#include "../imgproc/digitextractor.hpp"
#include "../classification/digitclassifier.hpp"

namespace Ui {
  class MainWindow;
}

class ProcessThread;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void printOnConsole(const QString &msg);

signals:
  void startWorking();
  void sudokuSolved();

public slots:
  void closing();

private slots:
  void saveCells();
  void trainClassifier();
//  void crossValidation();

  void saveClassifier();
  void loadClassifier();

  void updateCamView(QImage image);
  void updateSudokuView(size_t row, size_t col, uchar response);
  void fixSudokuView(size_t row, size_t col, uchar response);
  void clearSudokuView();

  void solveSudoku();

protected:

  bool askForTrainingSamples(std::vector<cv::Mat> *samples);

private:
  Ui::MainWindow *ui;

  QMutex _consoleLock;

  QImage _qFrame;

  QLCDNumber _digitViews[9][9];

  ProcessThread *_processThread;
  QThread *_thread;

  void setupSudokuGrid();

  void setSolutionDigit(size_t row, size_t col, uchar digit);
};

#endif // MAINWINDOW_HPP
