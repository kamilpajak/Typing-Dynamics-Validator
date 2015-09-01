#include "profile.h"

#include <cmath>

Profile::Profile(std::vector<Sample*> trainingSet) {
  this->trainingSet = trainingSet;
  double counter;

  // Means
  for (std::size_t i = 0; i < trainingSet[0]->getDownDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getDownDownIntervals()[i];
    this->downDownMeans.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0]->getUpDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getUpDownIntervals()[i];
    this->upDownMeans.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0]->getDownUpIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j]->getDownUpIntervals()[i];
    this->downUpMeans.push_back(counter / trainingSet.size());
  }

  // Standard deviations
  for (std::size_t i = 0; i < trainingSet[0]->getDownDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getDownDownIntervals()[i] - this->downDownMeans[i]);
    this->downDownStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0]->getUpDownIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getUpDownIntervals()[i] - this->upDownMeans[i]);
    this->upDownStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0]->getDownUpIntervals().size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j]->getDownUpIntervals()[i] - this->downUpMeans[i]);
    this->downUpStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  // Mean deviations
  counter = 0;
  for (std::size_t i = 0; i < this->downDownStandardDeviations.size(); i++)
    counter += this->downDownStandardDeviations[i];
  this->downDownMeanDeviation = counter / this->downDownStandardDeviations.size();

  counter = 0;
  for (std::size_t i = 0; i < this->upDownStandardDeviations.size(); i++)
    counter += this->upDownStandardDeviations[i];
  this->upDownMeanDeviation = counter / this->upDownStandardDeviations.size();

  counter = 0;
  for (std::size_t i = 0; i < this->downUpStandardDeviations.size(); i++)
    counter += this->downUpStandardDeviations[i];
  this->downUpMeanDeviation = counter / this->downUpStandardDeviations.size();
}

// Getters
std::vector<double> Profile::getDownDownMeans() const {
  return this->downDownMeans;
}

std::vector<double> Profile::getUpDownMeans() const {
  return this->upDownMeans;
}

std::vector<double> Profile::getDownUpMeans() const {
  return this->downUpMeans;
}

std::vector<double> Profile::getDownDownStandardDeviations() const {
  return this->downDownStandardDeviations;
}

std::vector<double> Profile::getUpDownStandardDeviations() const {
  return this->upDownStandardDeviations;
}

std::vector<double> Profile::getDownUpStandardDeviations() const {
  return this->downUpStandardDeviations;
}

std::vector<Sample*> Profile::getTrainingSet() const {
  return this->trainingSet;
}

double Profile::getDownDownMeanDeviation() const {
  return this->downDownMeanDeviation;
}

double Profile::getUpDownMeanDeviation() const {
  return this->upDownMeanDeviation;
}

double Profile::getDownUpMeanDeviation() const {
  return this->downUpMeanDeviation;
}
