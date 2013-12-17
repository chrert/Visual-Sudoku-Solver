#ifndef GEOMETRICUTILS_HPP__
#define GEOMETRICUTILS_HPP__

#include "typedefs.hpp"

#include <functional>
#include <algorithm>
#include <opencv2/core/core.hpp>


class GeometricUtils
{
public:
  
  static bool computeIntersect(const cv::Vec4f& a, const cv::Vec4f& b, cv::Point2f& intersection)
  {
    float x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    float x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
    
    float bx = x2 - x1;
    float by = y2 - y1;
    float dx = x4 - x3;
    float dy = y4 - y3; 
    float b_dot_d_perp = bx*dy - by*dx;
    if(b_dot_d_perp == 0) {
      intersection.x = -1.f;
      intersection.y = -1.f;
      return false;
    }
    float cx = x3-x1; 
    float cy = y3-y1;
    float t = (cx*dy - cy*dx) / b_dot_d_perp; 
    
    intersection.x = x1 + t*bx;
    intersection.y = y1 + t*by;
    return true;
  }

  static float length(const cv::Vec2f& a)
  {
    float x = a[0], y=a[1];
    return sqrt(x*x + y*y);
  }
  
  inline static float area(const cv::Vec2f& ib, const cv::Vec2f& ic)
  {
    return fabs(ib[0]*ic[1] - ib[1]*ic[0])/2.0f;
  }
  
  template<typename T>
  inline static cv::Point_<T> findCornerCenter(const Contour<T>& corners)
  {
      return std::accumulate(std::begin(corners), std::end(corners), 
                             cv::Point_<T>(0, 0)) * (1. / corners.size());
  }
  
  template<typename T, typename U>
  static void copyContour(const Contour<T>& src, Contour<U>& dst)
  {
    dst.clear();
    for (const auto& p : src)
      dst.emplace_back(p.x, p.y);
  }
  
  template<typename T>
  static void sortCorners(Contour<T>& corners)
  {
      const cv::Point_<T> center = findCornerCenter(corners);
      
      Contour<T> top, bot;
      for (const auto& corner : corners)
      {
          if (corner.y < center.y)
              top.emplace_back(corner);
          else
              bot.emplace_back(corner);
      }

      auto tl = top[0].x > top[1].x ? top[1] : top[0];
      auto tr = top[0].x > top[1].x ? top[0] : top[1];
      auto bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
      auto br = bot[0].x > bot[1].x ? bot[0] : bot[1];

      corners.clear();
      corners.emplace_back(tl);
      corners.emplace_back(tr);
      corners.emplace_back(br);
      corners.emplace_back(bl);
  }
  
  template<typename T>
  static Contour<float> fitQuadrilateral(Contour<T> convexHull)
  {
    Contour<float> ret;
    Contour<float> conv;
    copyContour(convexHull, ret);
    copyContour(convexHull, conv);
    
    while (ret.size()>4)
    {
      float minAddArea=std::numeric_limits<float>::infinity();
      int minAddAreaIndex=-1;
      int retSize = ret.size();
      for (int i=0; i<ret.size(); i++)
      {
        cv::Point2f a, b, c, d;
        a = ret[i%retSize];
        b = ret[(i+1)%retSize];
        c = ret[(i+2)%retSize];
        d = ret[(i+3)%retSize];
        
        cv::Vec4f ab(a.x, a.y, b.x, b.y);
        cv::Vec4f cd(c.x, c.y, d.x, d.y);
        
        cv::Point2f intersection;
        GeometricUtils::computeIntersect(ab, cd, intersection);
        
        cv::Vec2f ib = b - intersection;
        cv::Vec2f ic = c - intersection;
        
        float area = GeometricUtils::area(ib, ic);
        
        if (area < minAddArea)
        {
          minAddArea = area;
          minAddAreaIndex = i;
        }
      }
      
      if (minAddAreaIndex >= 0)
      {
        cv::Point2f a, b, c, d;
        a = ret[minAddAreaIndex%retSize];
        b = ret[(minAddAreaIndex+1)%retSize];
        c = ret[(minAddAreaIndex+2)%retSize];
        d = ret[(minAddAreaIndex+3)%retSize];
        
        cv::Vec4f ab(a.x, a.y, b.x, b.y);
        cv::Vec4f cd(c.x, c.y, d.x, d.y);
        
        cv::Point2f intersection;
        GeometricUtils::computeIntersect(ab, cd, intersection);
        
        if (minAddAreaIndex+1==ret.size())
          minAddAreaIndex=-1;
        ret.erase(ret.begin()+minAddAreaIndex+1);
        
        if (minAddAreaIndex+1==ret.size())
          minAddAreaIndex=-1;
        ret.erase(ret.begin()+minAddAreaIndex+1);
        
        if (minAddAreaIndex+1==ret.size())
          minAddAreaIndex=-1;
        ret.insert(ret.begin()+minAddAreaIndex+1, intersection);
        
      }
      else
      {
        throw "SERIOUS PROBLEM in Quadrilateraldetection";
      }
    }
    
    return ret;
  }
  
  
};


#endif
