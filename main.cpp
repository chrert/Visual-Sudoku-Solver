#include "sudokufinder.hpp"
#include "drawutils.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
    
  bool video = argc > 1;
  
  SudokuFinder sudokuFinder;
  
  cv::VideoCapture cap(0);
  while (cap.grab())
  {
    cv::Mat frame;
    cap >> frame;
    
    if (sudokuFinder.updateFrame(frame))
    {
      DrawUtils::drawContour(frame, sudokuFinder.getFoundSudokuContour(), cv::Scalar(0, 255, 0));
      cv::imshow("unwrap", sudokuFinder.getProjectedSudoku());
    }
    
    cv::imshow("frame", frame);
    
    if (cv::waitKey(100) >= 0)
      break;
  }
  
  return 0;
}
