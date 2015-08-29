#include "profile.h"

#include <cmath>

Profile::Profile(std::vector<Sample*> trainingSet) {
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

  this->trainingSet = trainingSet;
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
