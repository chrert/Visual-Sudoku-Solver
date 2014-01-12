#ifndef SUDOKU_FINDER_HPP__
#define SUDOKU_FINDER_HPP__

#include "../typedefs.hpp"
#include "../settings.hpp"

#include <opencv2/core/core.hpp>

class SudokuFinder
{
public:
  
  SudokuFinder(size_t cell_size);
  ~SudokuFinder();
  
  bool updateFrame(const cv::Mat& frame);
  
  const cv::Mat& getFrame() const;
  const cv::Mat& getRectifiedSudoku() const;

  void showSolution(const uchar solution[NUM_ROWS_CELLS][NUM_ROWS_CELLS]);
  void unshowSolution();
  
  size_t getCellSize() const;
  size_t getRectificationSize() const;
  
  bool cell(size_t row, size_t col, cv::Mat& cell) const;
  
  const Contour<int>& getFoundSudokuContour() const;
  
private:
  
  bool _found;

  bool _showSolution;
  uchar _solution[NUM_ROWS_CELLS][NUM_ROWS_CELLS];
  
  size_t _rectificationSize;
  size_t _cellSize;
  
  cv::Mat _frame;
  cv::Mat _preparedFrame;
  cv::Mat _rectifiedSudoku;
  
  Contour<int> _foundContour;
  
  Contour<float> _transformedRect;
  Contour<float> _perspectiveRect;
  cv::Mat _homography;
  
  void prepareFrame();

  bool findSudoku();
  void transformSudoku();

  void transformSolutionToFrame();
};

#endif
