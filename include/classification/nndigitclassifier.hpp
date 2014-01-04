#ifndef NNDIGITCLASSIFIER_HPP__
#define NNDIGITCLASSIFIER_HPP__

#include "digitclassifier.hpp"
#include "../imgproc/digitextractor.hpp"

#include <opencv2/ml/ml.hpp>

class NNDigitClassifier : public DigitClassifier
{
public:
  NNDigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t pcaComponents = 0);
  virtual ~NNDigitClassifier();

  virtual void train(std::vector<cv::Mat>* trainingImages);
  virtual uchar classify(const cv::Mat& image);

  virtual bool load(const std::string& filename);
  virtual bool save(const std::string& filename) const;

private:
  CvANN_MLP* _nn;
};

#endif // NNDIGITCLASSIFIER_HPP
