#include "sudokufinder.hpp"
#include "typedefs.hpp"
#include "geometricutils.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#define CANNY_LOW  40
#define CANNY_HIGH 80

#define MIN_CONTOUR_AREA 1000

#define NUM_ROWS_CELLS 9

SudokuFinder::SudokuFinder(size_t cell_size) : _frame(), 
                                               _preparedFrame(),
                                               _rectifiedSudoku(),
                                               _transformedRect(),
                                               _perspectiveRect()
{
  _cell_size = cell_size;
  _rectification_size = cell_size * NUM_ROWS_CELLS;
  
  _transformedRect.push_back(cv::Point2f(0, 0));
  _transformedRect.push_back(cv::Point2f(_rectification_size, 0));
  _transformedRect.push_back(cv::Point2f(_rectification_size, _rectification_size));
  _transformedRect.push_back(cv::Point2f(0, _rectification_size));
  GeometricUtils::sortCorners(_transformedRect);
}


SudokuFinder::~SudokuFinder()
{
}

size_t SudokuFinder::getCellSize() const
{
  return _cell_size;
}

size_t SudokuFinder::getRectificationSize() const
{
  return _rectification_size;
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

bool SudokuFinder::cell(size_t row, size_t col, cv::Mat& cell) const
{
  if (row >= NUM_ROWS_CELLS || col >= NUM_ROWS_CELLS || ! _found)
    return false;
  
  cv::Rect roi(col * _cell_size, row * _cell_size, _cell_size, _cell_size);
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

  return true;
}

void SudokuFinder::transformSudoku()
{
  cv::Mat homography = cv::findHomography(_perspectiveRect, _transformedRect, 0);
  cv::warpPerspective(_frame, _rectifiedSudoku, homography, cv::Size2f(_rectification_size, _rectification_size));
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
