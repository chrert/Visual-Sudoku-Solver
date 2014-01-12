#include "../../include/imgproc/sudokufinder.hpp"
#include "../../include/typedefs.hpp"
#include "../../include/settings.hpp"
#include "../../include/utils/geometricutils.hpp"
#include "../../include/utils/drawutils.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

SudokuFinder::SudokuFinder(size_t cell_size) : _frame(), 
                                               _preparedFrame(),
                                               _rectifiedSudoku(),
                                               _transformedRect(),
                                               _perspectiveRect()
{
  _cellSize = cell_size;
  _rectificationSize = cell_size * NUM_ROWS_CELLS;
  
  _transformedRect.push_back(cv::Point2f(0, 0));
  _transformedRect.push_back(cv::Point2f(_rectificationSize, 0));
  _transformedRect.push_back(cv::Point2f(_rectificationSize, _rectificationSize));
  _transformedRect.push_back(cv::Point2f(0, _rectificationSize));
  GeometricUtils::sortCorners(_transformedRect);

  unshowSolution();
}


SudokuFinder::~SudokuFinder()
{
}

size_t SudokuFinder::getCellSize() const
{
  return _cellSize;
}

size_t SudokuFinder::getRectificationSize() const
{
  return _rectificationSize;
}

const cv::Mat& SudokuFinder::getFrame() const
{
  return _frame;
}

const cv::Mat& SudokuFinder::getRectifiedSudoku() const
{
  return _rectifiedSudoku;
}

const Contour<int>& SudokuFinder::getFoundSudokuContour() const
{
  return _foundContour;
}

void SudokuFinder::showSolution(const uchar solution[NUM_ROWS_CELLS][NUM_ROWS_CELLS])
{
  memcpy(_solution, solution, NUM_ROWS_CELLS*NUM_ROWS_CELLS);
  _showSolution = true;
}

void SudokuFinder::unshowSolution()
{
  _showSolution = false;
}

bool SudokuFinder::cell(size_t row, size_t col, cv::Mat& cell) const
{
  if (row >= NUM_ROWS_CELLS || col >= NUM_ROWS_CELLS || ! _found)
    return false;
  
  cv::Rect roi(col * _cellSize, row * _cellSize, _cellSize, _cellSize);
  _rectifiedSudoku(roi).copyTo(cell);
  
  return true;
}


bool SudokuFinder::updateFrame(const cv::Mat& frame)
{
  _found = false;
  
  _frame = frame.clone();
  
  _foundContour.clear();
  _perspectiveRect.clear();
  
  prepareFrame();
  
  if (! (_found = findSudoku()))
    return false;
  
  transformSudoku();

  if (_showSolution)
    transformSolutionToFrame();

  return true;
}

void SudokuFinder::transformSolutionToFrame()
{
  cv::Mat solutionMat = cv::Mat::ones(_rectificationSize, _rectificationSize, CV_8UC3);
  for (size_t row = 0; row < NUM_ROWS_CELLS; ++row)
  {
    for (size_t col = 0; col < NUM_ROWS_CELLS; ++col)
    {
      uchar response = _solution[row][col];
      if (response != NO_DIGIT_FOUND)
      {
        std::stringstream ss;
        ss << (int) response;

        cv::Point p(col * SUDOKU_CELL_WORKING_SIZE + 5, row * SUDOKU_CELL_WORKING_SIZE + SUDOKU_CELL_WORKING_SIZE-5);
        cv::putText(solutionMat, ss.str(), p, CV_FONT_HERSHEY_COMPLEX_SMALL, 2, cv::Scalar(0,165,255));
      }
    }
  }
  cv::Mat digitFrame = _frame.clone();
  cv::warpPerspective(solutionMat, digitFrame, _homography, _frame.size(), cv::WARP_INVERSE_MAP);
  cv::Mat digitFrameMask;
  cv::cvtColor(digitFrame, digitFrameMask, CV_BGR2GRAY);
  cv::threshold(digitFrameMask, digitFrameMask, 100, 255, CV_THRESH_BINARY);
  digitFrame.copyTo(_frame, digitFrameMask);
}

void SudokuFinder::transformSudoku()
{
  _homography = cv::findHomography(_perspectiveRect, _transformedRect, 0);
  cv::warpPerspective(_frame, _rectifiedSudoku, _homography, cv::Size2f(_rectificationSize, _rectificationSize));
}

bool SudokuFinder::findSudoku()
{
  std::vector<Contour<int>> contours;
  cv::findContours(_preparedFrame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  
  if (contours.empty())
    return false;
  
  float maxArea = 0.f;
  size_t maxAreaIndex = 0;
  for (size_t i = 0; i < contours.size(); ++i)
  {
    float area = cv::contourArea(contours[i]);
    if (area >= maxArea)
    {
      maxArea = area;
      maxAreaIndex = i;
    }
  }
  
  if (maxArea < MIN_CONTOUR_AREA)
    return false;
  
  Contour<int> convexHull;
  cv::convexHull(contours[maxAreaIndex], convexHull);
  
  if (convexHull.size() < 4)
    return false;
  
  _foundContour = convexHull;
  
  _perspectiveRect = GeometricUtils::fitQuadrilateral(convexHull);
  GeometricUtils::sortCorners(_perspectiveRect);
  
  return true;
}
void SudokuFinder::prepareFrame()
{
  cv::cvtColor(_frame, _preparedFrame, CV_BGR2GRAY);
  cv::blur(_preparedFrame, _preparedFrame, cv::Size(3, 3));
  cv::Canny(_preparedFrame, _preparedFrame, CANNY_LOW, CANNY_HIGH);
}
