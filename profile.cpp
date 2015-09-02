#include "profile.h"

#include <cmath>

Profile::Profile(std::vector<Sample*> trainingSet) {
  this->trainingSet_ = trainingSet;
  double counter;

  // Means
  for (std::size_t i = 0; i < trainingSet[0]->getDownDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getDownDownIntervals()[i];
    this->downDownMeans_.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0]->getUpDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getUpDownIntervals()[i];
    this->upDownMeans_.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0]->getDownUpIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getDownUpIntervals()[i];
    this->downUpMeans_.push_back(counter / trainingSet.size());
  }

  // Standard deviations
  for (std::size_t i = 0; i < trainingSet[0]->getDownDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getDownDownIntervals()[i] - this->downDownMeans_[i]);
    this->downDownStandardDeviations_.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0]->getUpDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getUpDownIntervals()[i] - this->upDownMeans_[i]);
    this->upDownStandardDeviations_.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0]->getDownUpIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getDownUpIntervals()[i] - this->downUpMeans_[i]);
    this->downUpStandardDeviations_.push_back(counter / (trainingSet.size() - 1));
  }

  // Mean deviations
  counter = 0;
  for (std::size_t i = 0; i < this->downDownStandardDeviations_.size(); i++)
    counter += this->downDownStandardDeviations_[i];
  this->downDownMeanDeviation_ = counter / this->downDownStandardDeviations_.size();

  counter = 0;
  for (std::size_t i = 0; i < this->upDownStandardDeviations_.size(); i++)
    counter += this->upDownStandardDeviations_[i];
  this->upDownMeanDeviation_ = counter / this->upDownStandardDeviations_.size();

  counter = 0;
  for (std::size_t i = 0; i < this->downUpStandardDeviations_.size(); i++)
    counter += this->downUpStandardDeviations_[i];
  this->downUpMeanDeviation_ = counter / this->downUpStandardDeviations_.size();
}

// Getters
std::vector<double> Profile::getDownDownMeans() const {
  return this->downDownMeans_;
}

std::vector<double> Profile::getUpDownMeans() const {
  return this->upDownMeans_;
}

std::vector<double> Profile::getDownUpMeans() const {
  return this->downUpMeans_;
}

std::vector<double> Profile::getDownDownStandardDeviations() const {
  return this->downDownStandardDeviations_;
}

std::vector<double> Profile::getUpDownStandardDeviations() const {
  return this->upDownStandardDeviations_;
}

std::vector<double> Profile::getDownUpStandardDeviations() const {
  return this->downUpStandardDeviations_;
}

std::vector<Sample*> Profile::getTrainingSet() const {
  return this->trainingSet_;
}

double Profile::getDownDownMeanDeviation() const {
  return this->downDownMeanDeviation_;
}

double Profile::getUpDownMeanDeviation() const {
  return this->upDownMeanDeviation_;
}

double Profile::getDownUpMeanDeviation() const {
  return this->downUpMeanDeviation_;
}
