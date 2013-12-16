#ifndef DRAWUTILS_HPP__
#define DRAWUTILS_HPP__

#include "typedefs.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class DrawUtils
{
public:
  
  static const cv::Scalar COLOR_WHITE;
  static const cv::Scalar COLOR_BLACK;
  static const cv::Scalar COLOR_BLUE;
  static const cv::Scalar COLOR_GREEN;
  static const cv::Scalar COLOR_RED;
  
  static void drawRotatedRect(cv::Mat& img, const cv::RotatedRect& rect, const cv::Scalar& color)
  {
    cv::Point2f points[4];
    rect.points(points);
    
    cv::line(img, points[0], points[1], color);
    cv::line(img, points[1], points[2], color);
    cv::line(img, points[2], points[3], color);
    cv::line(img, points[3], points[0], color);
    
    cv::line(img, points[0], points[2], color);
    cv::line(img, points[1], points[3], color);
  }
  
  template<typename T>
  static void drawContour(cv::Mat& img, const Contour<T>& contour, const cv::Scalar& color, bool fill = false)
  {
    std::vector<Contour<T>> tmp;
    tmp.push_back(contour);
    cv::drawContours(img, tmp, 0, color, fill ? CV_FILLED : 1);
  }
  
};

#endif
