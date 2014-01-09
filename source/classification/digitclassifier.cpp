#include "../../include/classification/digitclassifier.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

DigitClassifier::DigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t pcaComponents)
  : _extractor(extractor),
    _sampleWidth(sampleWidth),
    _pcaComponents(pcaComponents),
    _pca(nullptr)
{
}

DigitClassifier::~DigitClassifier()
{
  if (_pca) delete _pca;
}

template<typename T>
cv::Mat DigitClassifier::toRowVector(const cv::Mat& in) const
{
  cv::Mat out;
  out.create(1, in.rows*in.cols, in.type());
  int ii = 0;
  for (int i = 0; i < in.rows; i++) {
    for (int j = 0; j < in.cols; j++) {
        out.at<T>(0,ii++) = in.at<T>(i,j);
    }
  }
  return out;
}

cv::Mat DigitClassifier::prepareDigitMat(const cv::Mat& in, bool pca) const
{
  cv::Mat tmp;
  _extractor.extractDigit(in, tmp);

  std::vector<Contour<int>> contours;
  Contour<int> contour;
  cv::findContours(tmp.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  for (auto c : contours)
    for (auto p : c)
      contour.push_back(p);

  if (contour.size() > 1)
  {
    cv::Rect boundingBox = cv::boundingRect(contour);
    tmp = tmp(boundingBox);
  }

  cv::resize(tmp, tmp, cv::Size(_sampleWidth, _sampleWidth));

  tmp.convertTo(tmp, CV_32FC1, 1.0/255.0);
  cv::Mat row = toRowVector<float>(tmp);
  if (! pca || ! usePCA())
    return row;
  else
    return _pca->project(row);
}

void DigitClassifier::prepareTrainingMat(const std::vector<cv::Mat>* trainingImages, cv::Mat& trainingMat, cv::Mat& labelMat)
{
  size_t w = _sampleWidth * _sampleWidth;
  size_t num_samples = 0;
  for (int i = 0; i < 9; ++i)
    num_samples += trainingImages[i].size();

  cv::Mat pcaMat;
  pcaMat.create(num_samples, w, CV_32FC1);
  labelMat.create(num_samples, 1, CV_32SC1);

  size_t row = 0;
  for (int i = 0; i < 9; ++i)
  {
    auto it = std::begin(trainingImages[i]);
    for (; it != std::end(trainingImages[i]); ++it)
    {
      cv::Mat prepared = prepareDigitMat(*it, false);

      for (int j = 0; j < w; ++j)
        pcaMat.at<float>(row, j) = prepared.at<float>(0, j);
      labelMat.at<int>(row++, 0) = i+1;
    }
  }

  if (! usePCA())
  {
    pcaMat.copyTo(trainingMat);
  }
  else
  {
    if (_pca) delete _pca;

    _pca = new cv::PCA(pcaMat, cv::noArray(), CV_PCA_DATA_AS_ROW, static_cast<int>(_pcaComponents));
    trainingMat.create(pcaMat.rows, _pcaComponents, CV_32FC1);
    for (int i = 0; i < pcaMat.rows; ++i)
    {
      cv::Mat pcaRow = pcaMat.row(i);
      cv::Mat reduced = _pca->project(pcaRow);
      for (int j = 0;  j < reduced.cols; ++j)
        trainingMat.at<float>(i, j) = reduced.at<float>(0, j);
    }
  }
}

size_t DigitClassifier::getSampleWidth() const
{
  return _sampleWidth;
}

size_t DigitClassifier::getPCAComponents() const
{
  return _sampleWidth;
}

bool DigitClassifier::usePCA() const
{
  return _pcaComponents > 0;
}
