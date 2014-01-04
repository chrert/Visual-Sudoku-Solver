#include "../../include/classification/nndigitclassifier.hpp"

NNDigitClassifier::NNDigitClassifier(const DigitExtractor& extractor, size_t sampleWidth, size_t pcaComponents)
  : DigitClassifier(extractor, sampleWidth, pcaComponents),
    _nn(nullptr)
{
}

NNDigitClassifier::~NNDigitClassifier()
{
}

void NNDigitClassifier::train(std::vector<cv::Mat>* trainingImages)
{
  cv::Mat layers(1, 3, CV_32SC1);
  layers.at<int>(0,0) = getSampleWidth() * getSampleWidth();
  layers.at<int>(0,1) = 35;
  layers.at<int>(0,2) = 9;
  _nn = new CvANN_MLP(layers);

  cv::Mat trainingMat, labelMat;
  prepareTrainingMat(trainingImages, trainingMat, labelMat);

  cv::Mat outputVector(labelMat.rows, 9, CV_32FC1);
  for (size_t row = 0; row < labelMat.rows; ++row)
  {
    for (size_t digit = 1; digit <= 9; ++digit)
    {
      if (digit == labelMat.at<float>(row, 0))
        outputVector.at<float>(row, digit-1) = 1.f;
      else
        outputVector.at<int>(row, digit-1) = 0.f;
    }
  }

  _nn->train(trainingMat, outputVector, cv::Mat());
}

uchar NNDigitClassifier::classify(const cv::Mat& image)
{
  if (! _nn)
    return NO_DIGIT_FOUND;

  cv::Mat prepared = prepareDigitMat(image);
  cv::Mat response;
  _nn->predict(prepared, response);
  cv::Point maxDigit;
  cv::minMaxLoc(response, nullptr, nullptr, nullptr, &maxDigit);

  return static_cast<uchar>(maxDigit.x + 1);
}

bool NNDigitClassifier::load(const std::string &filename)
{
  return false;
}

bool NNDigitClassifier::save(const std::string &filename) const
{
  return false;
}
