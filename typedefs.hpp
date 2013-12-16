#ifndef TYPEDEFS_HPP__
#define TYPEDEFS_HPP__

#include <vector>
#include <opencv2/core/core.hpp>

template<typename T>
using Contour = std::vector<cv::Point_<T>>;

#endif
