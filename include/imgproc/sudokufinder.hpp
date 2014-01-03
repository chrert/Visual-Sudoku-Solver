#ifndef SUDOKU_FINDER_HPP__
#define SUDOKU_FINDER_HPP__

#include "../typedefs.hpp"

#include <opencv2/core/core.hpp>

class SudokuFinder
{
public:
  
  SudokuFinder(size_t cell_size);
  ~SudokuFinder();
  
  bool updateFrame(const cv::Mat& frame);
  
  const cv::Mat& getFrame() const;
  const cv::Mat& getRectifiedSudoku() const;
  
  size_t getCellSize() const;
  size_t getRectificationSize() const;
  
  bool cell(size_t row, size_t col, cv::Mat& cell) const;
  
  const Contour<int>& getFoundSudokuContour() const;
  
private:
  
  bool _found;
  
  size_t _rectificationSize;
  size_t _cellSize;
  
  cv::Mat _frame;
  cv::Mat _preparedFrame;
  cv::Mat _rectifiedSudoku;
  
  Contour<int> _foundContour;
  
  Contour<float> _transformedRect;
  Contour<float> _perspectiveRect;
  
  void prepareFrame();

  bool findSudoku();
  void transformSudoku();
};

#endif
