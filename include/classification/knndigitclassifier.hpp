#ifndef KNNDIGITCLASSIFIER_HPP__
#define KNNDIGITCLASSIFIER_HPP__

#include "digitclassifier.hpp"
#include "../imgproc/digitextractor.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>

#include <vector>

class KNNDigitClassifier : public DigitClassifier
{
public:
  KNNDigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t k, size_t pcaComponents = 0);
  virtual ~KNNDigitClassifier();

  virtual void train(std::vector<cv::Mat>* trainingImages);
  virtual uchar classifiy(const cv::Mat& image);

  virtual bool load(const std::string& filename);
  virtual bool save(const std::string& filename) const;

private:
  cv::KNearest* _knn;
  size_t _k;

  void create();
  void cleanup();
};

#endif // KNNDIGITCLASSIFIER_HPP
