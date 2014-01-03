#ifndef DIGITEXTRACTOR_HPP__
#define DIGITEXTRACTOR_HPP__

#include "../imgproc/sudokufinder.hpp"
#include "../settings.hpp"

#include <opencv2/core/core.hpp>

class DigitExtractor
{
public:

  DigitExtractor(const SudokuFinder& sudokuFinder);

  bool containsDigit(size_t row, size_t col) const;
  bool cell(size_t row, size_t col, cv::Mat& cell) const;

  bool updateCells();

  void extractDigit(const cv::Mat& src, cv::Mat& digit) const;

private:

  bool _prepared;
  cv::Mat _digits[NUM_ROWS_CELLS][NUM_ROWS_CELLS];

  int _thresholdKSize;
  cv::Rect _searchRegion;

  const SudokuFinder& _sudokuFinder;

  bool updateCell(size_t row, size_t col);

  void floodExtract(const cv::Mat& src, cv::Mat& dst, const cv::Mat& startPixel, const cv::Rect& startRect) const;
  void floodExtract(const cv::Mat& src, cv::Mat& dst, cv::Mat& visited, int row, int col) const;

  void deskew(cv::Mat& digit) const;
  void moveToCenter(cv::Mat& digit) const;

  bool containsDigit(const cv::Mat& img) const;
};

#endif // DIGITEXTRACTOR_HPP
