#include "classifier.h"

Classifier::Classifier(Profile* profile, Sample* sample) {
  // Distances
  this->downDownDistance_ = 0;
  this->upDownDistance_ = 0;
  this->downUpDistance_ = 0;

  for (std::size_t i = 0; i < sample->getDownDownIntervals().size(); i++)
    this->downDownDistance_ += (sample->getDownDownIntervals()[i] - profile->getDownDownMeans()[i]) / profile->getDownDownStandardDeviations()[i];
  this->downDownDistance_ /= sample->getDownDownIntervals().size();

  for (std::size_t i = 0; i < sample->getUpDownIntervals().size(); i++)
    this->upDownDistance_ += (sample->getUpDownIntervals()[i] - profile->getUpDownMeans()[i]) / profile->getUpDownStandardDeviations()[i];
  this->upDownDistance_ /= sample->getUpDownIntervals().size();

  for (std::size_t i = 0; i < sample->getDownUpIntervals().size(); i++)
    this->downUpDistance_ += (sample->getDownUpIntervals()[i] - profile->getDownUpMeans()[i]) / profile->getDownUpStandardDeviations()[i];
  this->downUpDistance_ /= sample->getDownUpIntervals().size();

  // Thresholds
  this->downDownThreshold_ = profile->getDownDownMeanDeviation() * 100;
  this->upDownThreshold_ = profile->getUpDownMeanDeviation() * 100;
  this->downUpThreshold_ = profile->getDownUpMeanDeviation() * 100;

  // Validate
  this->isValid_ = (this->downDownDistance_ <= this->downDownThreshold_ &&
                    this->upDownDistance_ <= this->upDownThreshold_ &&
                    this->downUpDistance_ <= this->downUpThreshold_);
}

// Getters
double Classifier::getDownDownDistance() const {
  return this->downDownDistance_;
}

double Classifier::getUpDownDistance() const {
  return this->upDownDistance_;
}

double Classifier::getDownUpDistance() const {
  return this->downUpDistance_;
}

double Classifier::getDownDownThreshold() const {
  return this->downDownThreshold_;
}

double Classifier::getUpDownThreshold() const {
  return this->upDownThreshold_;
}

double Classifier::getDownUpThreshold() const {
  return this->downUpThreshold_;
}

bool Classifier::isValid() const {
  return this->isValid_;
}
