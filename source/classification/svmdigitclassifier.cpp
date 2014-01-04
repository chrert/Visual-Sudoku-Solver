#include "../../include/classification/svmdigitclassifier.hpp"

SVMDigitClassifier::SVMDigitClassifier(const DigitExtractor& extractor, size_t sample_width, size_t pcaComponents)
  : DigitClassifier(extractor, sample_width, pcaComponents),
    _svm(nullptr)
{
}

SVMDigitClassifier::~SVMDigitClassifier()
{
  cleanup();
}

void SVMDigitClassifier::train(std::vector<cv::Mat>* trainingImages)
{
  cleanup();

  cv::Mat trainingMat, labelMat;
  prepareTrainingMat(trainingImages, trainingMat, labelMat);

  cv::SVMParams params = create();
  _svm->train_auto(trainingMat, labelMat, cv::Mat(), cv::Mat(), params);
}

uchar SVMDigitClassifier::classify(const cv::Mat& image)
{
  if (! _svm)
    return NO_DIGIT_FOUND;

  cv::Mat prepared = prepareDigitMat(image);
  float response = _svm->predict(prepared);

  return static_cast<uchar>(response);
}

bool SVMDigitClassifier::load(const std::string& filename)
{
  cleanup();
  create();
  try
  {
    _svm->load(filename.c_str());
    return true;
  }
  catch (...)
  {
    return false;
  }
}

bool SVMDigitClassifier::save(const std::string& filename) const
{
  try
  {
    if (_svm)
    {
      _svm->save(filename.c_str());
      return true;
    }
    else
    {
      return false;
    }
  }
  catch (...)
  {
    return false;
  }
}

cv::SVMParams SVMDigitClassifier::create()
{
  cv::SVMParams svm_params;
  svm_params.svm_type = cv::SVM::C_SVC;
  svm_params.kernel_type = cv::SVM::RBF;

  _svm = new cv::SVM();

  return svm_params;
}

void SVMDigitClassifier::cleanup()
{
  if (_svm)
    delete _svm;
}
