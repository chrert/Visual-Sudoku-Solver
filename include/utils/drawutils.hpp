#ifndef DRAWUTILS_HPP__
#define DRAWUTILS_HPP__

#include "../typedefs.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class DrawUtils
{
public:
  
  static const Color COLOR_WHITE;
  static const Color COLOR_BLACK;
  static const Color COLOR_BLUE;
  static const Color COLOR_GREEN;
  static const Color COLOR_RED;
  
  static void drawRotatedRect(cv::Mat& img, const cv::RotatedRect& rect, const Color& color)
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
  static void drawContour(cv::Mat& img, const Contour<T>& contour, const Color& color, unsigned int thickness = 1, bool fill = false)
  {
    std::vector<Contour<T>> tmp;
    tmp.push_back(contour);
    cv::drawContours(img, tmp, 0, color, fill ? CV_FILLED : thickness);
  }
};

#endif
