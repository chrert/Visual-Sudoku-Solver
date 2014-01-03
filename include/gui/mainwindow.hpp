#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QImage>
#include <QTimer>
#include <QString>
#include <QLabel>
#include <QMutex>

#include <opencv2/highgui/highgui.hpp>

#include "../imgproc/sudokufinder.hpp"
#include "../imgproc/digitextractor.hpp"
#include "../classification/digitclassifier.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void process();

  void saveCells();
  void trainClassifier();
  void crossValidation();

  void saveClassifier();
  void loadClassifier();

protected:
  void updateCamView(const cv::Mat& mat);
  void updateSudokuView();

  void printOnConsole(const QString& msg);

  bool askForTrainingSamples(std::vector<cv::Mat>* samples);

private:
  Ui::MainWindow *ui;

  cv::VideoCapture _cap;

  SudokuFinder _sudokuFinder;
  DigitExtractor _digitExtractor;
  DigitClassifier* _digitClassifier;

  bool _classify;

  QTimer* _processTimer;
  QMutex _processMutex;

  QImage _qFrame;

  QImage  _sudokuCells[9][9];
  QLabel* _sudokuViews[9][9];

  void setupSudokuGrid();
};

#endif // MAINWINDOW_HPP
