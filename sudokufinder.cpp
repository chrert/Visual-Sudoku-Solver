#include "sudokufinder.hpp"
#include "typedefs.hpp"
#include "geometricutils.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <boost/concept_check.hpp>

#define CANNY_LOW  40
#define CANNY_HIGH 80

#define MIN_CONTOUR_AREA 1000

#define TRANSFORMED_LEN 400

SudokuFinder::SudokuFinder() : _frame(), 
                               _preparedFrame(),
                               _projectedSudoku(),
                               _transformedRect(),
                               _perspectiveRect()
{
  _transformedRect.push_back(cv::Point2f(0, 0));
  _transformedRect.push_back(cv::Point2f(TRANSFORMED_LEN, 0));
  _transformedRect.push_back(cv::Point2f(TRANSFORMED_LEN, TRANSFORMED_LEN));
  _transformedRect.push_back(cv::Point2f(0, TRANSFORMED_LEN));
  GeometricUtils::sortCorners(_transformedRect);
}


SudokuFinder::~SudokuFinder()
{
}

const cv::Mat& SudokuFinder::getFrame() const
{
  return _frame;
}

const cv::Mat& SudokuFinder::getProjectedSudoku() const
{
  return _projectedSudoku;
}

const Contour& SudokuFinder::getFoundSudokuContour() const
{
  return _foundContour;
}


bool SudokuFinder::updateFrame(const cv::Mat& frame)
{
  _frame = frame.clone();
  
  _foundContour.clear();
  _perspectiveRect.clear();
  
  prepareFrame();
  
  if (! findSudoku())
    return false;
  
  transformSudoku();
}

void SudokuFinder::transformSudoku()
{
  cv::Mat homography = cv::findHomography(_perspectiveRect, _transformedRect, 0);
  cv::warpPerspective(_frame, _projectedSudoku, homography, cv::Size2f(TRANSFORMED_LEN, TRANSFORMED_LEN));
}

bool SudokuFinder::findSudoku()
{
  std::vector<Contour> contours;
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
  
  Contour convexHull;
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