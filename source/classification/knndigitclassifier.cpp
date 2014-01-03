#include "../../include/classification/knndigitclassifier.hpp"

KNNDigitClassifier::KNNDigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t k, size_t pcaComponents)
  : DigitClassifier(extractor, sampleWidth, pcaComponents),
    _knn(nullptr),
    _k(k)
{
}

KNNDigitClassifier::~KNNDigitClassifier()
{
  cleanup();
}

void KNNDigitClassifier::train(std::vector<cv::Mat>* trainingImages)
{
  create();

  cv::Mat trainingMat, labelMat;
  prepareTrainingMat(trainingImages, trainingMat, labelMat);
  _knn->train(trainingMat, labelMat);
}

uchar KNNDigitClassifier::classifiy(const cv::Mat& image)
{
  if (! _knn)
    return 0;

  cv::Mat prepared = prepareDigitMat(image);
  float response = _knn->find_nearest(prepared, std::min(_k, static_cast<size_t>(_knn->get_max_k())));

  return static_cast<uchar>(response);
}

bool KNNDigitClassifier::load(const std::string& filename)
{
  return false;
}

bool KNNDigitClassifier::save(const std::string& filename) const
{
  return false;
}

void KNNDigitClassifier::create()
{
  _knn = new cv::KNearest;
}

void KNNDigitClassifier::cleanup()
{
  if (_knn) delete _knn;
}
