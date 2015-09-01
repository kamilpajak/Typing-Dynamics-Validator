#include "classifier.h"

Classifier::Classifier(Profile* profile, Sample* sample) {
  // Distances
  this->downDownDistance = 0;
  this->upDownDistance = 0;
  this->downUpDistance = 0;

  for (std::size_t i = 0; i < sample->getDownDownIntervals().size(); i++)
    this->downDownDistance += (sample->getDownDownIntervals()[i] - profile->getDownDownMeans()[i]) / profile->getDownDownStandardDeviations()[i];
  this->downDownDistance /= sample->getDownDownIntervals().size();

  for (std::size_t i = 0; i < sample->getUpDownIntervals().size(); i++)
    this->upDownDistance += (sample->getUpDownIntervals()[i] - profile->getUpDownMeans()[i]) / profile->getUpDownStandardDeviations()[i];
  this->upDownDistance /= sample->getUpDownIntervals().size();

  for (std::size_t i = 0; i < sample->getDownUpIntervals().size(); i++)
    this->downUpDistance += (sample->getDownUpIntervals()[i] - profile->getDownUpMeans()[i]) / profile->getDownUpStandardDeviations()[i];
  this->downUpDistance /= sample->getDownUpIntervals().size();

  // Thresholds
  this->downDownThreshold = profile->getDownDownMeanDeviation() * 100;
  this->upDownThreshold = profile->getUpDownMeanDeviation() * 100;
  this->downUpThreshold = profile->getDownUpMeanDeviation() * 100;

  // Validate
  this->isValid = (this->downDownDistance <= this->downDownThreshold &&
                   this->upDownDistance <= this->upDownThreshold &&
                   this->downUpDistance <= this->downUpThreshold);
}

// Getters
double Classifier::getDownDownDistance() const {
  return this->downDownDistance;
}

double Classifier::getUpDownDistance() const {
  return this->upDownDistance;
}

double Classifier::getDownUpDistance() const {
  return this->downUpDistance;
}

double Classifier::getDownDownThreshold() const {
  return this->downDownThreshold;
}

double Classifier::getUpDownThreshold() const {
  return this->upDownThreshold;
}

double Classifier::getDownUpThreshold() const {
  return this->downUpThreshold;
}

bool Classifier::isValidSample() const {
  return this->isValid;
}
