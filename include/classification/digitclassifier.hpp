#ifndef DIGITCLASSIFIER_HPP__
#define DIGITCLASSIFIER_HPP__

#include <opencv2/core/core.hpp>

#include <vector>

#include "../imgproc/digitextractor.hpp"

class DigitClassifier
{
public:

  DigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t pcaComponents = 0);
  virtual ~DigitClassifier();

  virtual void train(const std::vector<cv::Mat>* trainingImages) = 0;
  virtual uchar classify(const cv::Mat& image) = 0;

  virtual bool save(const std::string& filename) const = 0;
  virtual bool load(const std::string& filename) = 0;

  size_t getSampleWidth() const;

  bool usePCA() const;
  size_t getPCAComponents() const;

protected:
  cv::Mat prepareDigitMat(const cv::Mat& in, bool pca = true) const;
  void prepareTrainingMat(const std::vector<cv::Mat>* trainingImages, cv::Mat& trainingMat, cv::Mat& labelMat);

private:
  const DigitExtractor &_extractor;

  template<typename T>
  cv::Mat toRowVector(const cv::Mat& in) const;

  size_t _sampleWidth;

  size_t _pcaComponents;
  cv::PCA *_pca;
};

#endif // DIGITCLASSIFIER_HPP
