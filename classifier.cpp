#include "classifier.h"

#include <cmath>

Classifier::Classifier(Profile* profile, Sample* sample) {
  // Distances
  this->downDownDistance_ = 0;
  this->upDownDistance_ = 0;
  this->downUpDistance_ = 0;

  for (std::size_t i = 0; i < sample->getDownDownIntervals().size(); i++)
    this->downDownDistance_ += std::abs(sample->getDownDownIntervals()[i] - profile->getDownDownMeans()[i]) / profile->getDownDownStandardDeviations()[i];
  this->downDownDistance_ /= sample->getDownDownIntervals().size();

  for (std::size_t i = 0; i < sample->getUpDownIntervals().size(); i++)
    this->upDownDistance_ += std::abs(sample->getUpDownIntervals()[i] - profile->getUpDownMeans()[i]) / profile->getUpDownStandardDeviations()[i];
  this->upDownDistance_ /= sample->getUpDownIntervals().size();

  for (std::size_t i = 0; i < sample->getDownUpIntervals().size(); i++)
    this->downUpDistance_ += std::abs(sample->getDownUpIntervals()[i] - profile->getDownUpMeans()[i]) / profile->getDownUpStandardDeviations()[i];
  this->downUpDistance_ /= sample->getDownUpIntervals().size();
}

// Setters
void Classifier::setDownDownThreshold(Profile* profile, double a, double b) {
  this->downDownThreshold_ = a * profile->getDownDownMeanDeviation() + b;
}

void Classifier::setUpDownThreshold(Profile* profile, double a, double b) {
  this->upDownThreshold_ = a * profile->getUpDownMeanDeviation() + b;
}

void Classifier::setDownUpThreshold(Profile* profile, double a, double b) {
  this->downUpThreshold_ = a * profile->getDownUpMeanDeviation() + b;
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

bool Classifier::isValid() {
  return this->isDownDownValid() && this->isUpDownValid() && this->isDownUpValid();
}

bool Classifier::isDownDownValid() {
  return this->downDownDistance_ <= this->downDownThreshold_;
}

bool Classifier::isUpDownValid() {
  return this->upDownDistance_ <= this->upDownThreshold_;
}

bool Classifier::isDownUpValid() {
  return this->downUpDistance_ <= this->downUpThreshold_;
}
