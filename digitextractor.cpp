#include "digitextractor.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define THRESHOLD_C 10
#define THRESHOLD_SIZE 9

DigitExtractor::DigitExtractor(const SudokuFinder& sudokuFinder)
  : _sudokuFinder(sudokuFinder),
    _prepared(false)
{
  _thresholdKSize = _sudokuFinder.getCellSize();
  if (_thresholdKSize % 2 == 0)
    ++_thresholdKSize;

  int cellSize = _sudokuFinder.getCellSize();

  _searchRegion = cv::Rect(cellSize / 2, cellSize / 2,
                           cellSize / 4, cellSize / 4);
}

bool DigitExtractor::containsDigit(size_t row, size_t col) const
{
  if (row >= NUM_ROWS_CELLS || col >= NUM_ROWS_CELLS || ! _prepared)
    return false;

  return containsDigit(_digits[row][col]);
}

bool DigitExtractor::containsDigit(const cv::Mat& img) const
{
  int c = cv::countNonZero(img);
  return c > 0;
}

bool DigitExtractor::cell(size_t row, size_t col, cv::Mat& cell) const
{
  if (row >= NUM_ROWS_CELLS || col >= NUM_ROWS_CELLS || ! _prepared)
    return false;

  _digits[row][col].copyTo(cell);

  return true;
}

bool DigitExtractor::updateCells()
{
  _prepared = true;

  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
      if (! (_prepared = updateCell(row, col)))
        return false;

  return _prepared;
}

bool DigitExtractor::updateCell(size_t row, size_t col)
{
  cv::Mat cell;
  if (! _sudokuFinder.cell(row, col, cell))
    return false;

  cv::Mat& digit = _digits[row][col] = cv::Mat::zeros(_sudokuFinder.getCellSize(),
                                                      _sudokuFinder.getCellSize(),
                                                      CV_8UC1);

  extractDigit(cell, digit);

  return true;
}

void DigitExtractor::extractDigit(const cv::Mat& src, cv::Mat& digit) const
{
  cv::Mat cell = src.clone();
  cv::cvtColor(cell, cell, CV_BGR2GRAY);

  cv::adaptiveThreshold(cell, cell, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                        cv::THRESH_BINARY_INV, THRESHOLD_SIZE, THRESHOLD_C);

  digit = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  cv::Mat startPixel = cell(_searchRegion).clone();
  floodExtract(cell, digit, startPixel, _searchRegion);

  if (containsDigit(digit))
  {
    deskew(digit);
    moveToCenter(digit);
  }
}

void DigitExtractor::floodExtract(const cv::Mat& src, cv::Mat& dst,
                                  const cv::Mat& startPixel, const cv::Rect& startRect) const
{
  cv::Mat visited = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  for (int row = 0; row < startPixel.rows; ++row)
  {
    int y = startRect.y + row;
    for (int col = 0; col < startPixel.cols; ++col)
    {
      int x = startRect.x + col;
      if (startPixel.at<uchar>(row, col) > 0)
        floodExtract(src, dst, visited, y, x);
    }
  }
}

void DigitExtractor::floodExtract(const cv::Mat &src, cv::Mat &dst, cv::Mat &visited, int row, int col) const
{
  uchar visited_val = visited.at<uchar>(row, col);
  if (visited_val == 1)
    return;
  else
    visited.at<uchar>(row, col) = 1;

  uchar src_val = src.at<uchar>(row, col);
  if (src_val == 0)
    return;

  dst.at<uchar>(row, col) = 255;

  int w = dst.cols;
  int h = dst.rows;

  for (int x_n = -1; x_n <= 1; ++x_n)
  {
    int x = col + x_n;

    if (x < 0)
      continue;
    if (x >= w)
      break;

    for (int y_n = -1; y_n <= 1; ++y_n)
    {
      int y = row + y_n;

      if (y < 0)
        continue;
      if (y >= h)
        break;

      floodExtract(src, dst, visited, y, x);
    }
  }
}

void DigitExtractor::deskew(cv::Mat& digit) const
{
  size_t cellSize = _sudokuFinder.getCellSize();
  cv::Moments moments = cv::moments(digit, true);
  double skew = moments.mu11 / moments.mu02;
  double m[2][3] = {
    {1.0, skew, -0.5*cellSize*skew},
    {0.0, 1.0,   0.0}
  };
  cv::Mat M = cv::Mat(2, 3, CV_64FC1, &m);
  cv::warpAffine(digit, digit, M, cv::Size(cellSize, cellSize),
                 cv::WARP_INVERSE_MAP | cv::INTER_LINEAR);
}

void DigitExtractor::moveToCenter(cv::Mat& digit) const
{
  size_t cellSize = _sudokuFinder.getCellSize();
  cv::Moments moments = cv::moments(digit, true);
  double c_x = moments.m10 / moments.m00 - cellSize / 2.0;
  double c_y = moments.m01 / moments.m00 - cellSize / 2.0;
  double m[2][3] = {
    {1.0, 0.0, c_x},
    {0.0, 1.0, c_y}
  };
  cv::Mat M = cv::Mat(2, 3, CV_64FC1, &m);
  cv::warpAffine(digit, digit, M, cv::Size(cellSize, cellSize),
                 cv::WARP_INVERSE_MAP | cv::INTER_LINEAR);
  }
