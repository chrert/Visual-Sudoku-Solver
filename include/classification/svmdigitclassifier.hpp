#ifndef SVMDIGITCLASSIFIER_HPP__
#define SVMDIGITCLASSIFIER_HPP__

#include "digitclassifier.hpp"
#include "../imgproc/digitextractor.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include <vector>

class SVMDigitClassifier : public DigitClassifier
{
public:
  SVMDigitClassifier(const DigitExtractor& extractor, size_t sample_width, size_t pcaComponents = 0);
  virtual ~SVMDigitClassifier();

  virtual void train(std::vector<cv::Mat>* trainingImages);
  virtual uchar classifiy(const cv::Mat& image);

  virtual bool save(const std::string& filename) const;
  virtual bool load(const std::string& filename);

private:
  cv::SVM* _svm;

  cv::SVMParams create();
  void cleanup();
};

#endif // SVMDIGITCLASSIFIER_HPP
