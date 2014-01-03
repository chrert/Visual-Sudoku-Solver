#ifndef QTOPENCV_HPP__
#define QTOPENCV_HPP__

#include <opencv2/core/core.hpp>
#include <QImage>

class QtOpenCV
{
public:

  static QImage MatToQImage(const cv::Mat& mat, QImage::Format format)
  {
    return QImage(mat.data, mat.cols, mat.rows, format);
  }
};

#endif // QTOPENCV_HPP

