#include "distance.h"

Distance::Distance(Profile* profile, Sample* sample) {
  this->downDown = 0;
  this->upDown = 0;
  this->downDown = 0;

  for (std::size_t i = 0; i < sample->getDownDownIntervals().size(); i++)
    this->downDown += (sample->getDownDownIntervals()[i] - profile->getDownDownMeans()[i]) / profile->getDownDownStandardDeviations()[i];
  this->downDown /= sample->getDownDownIntervals().size();

  for (std::size_t i = 0; i < sample->getUpDownIntervals().size(); i++)
    this->upDown += (sample->getUpDownIntervals()[i] - profile->getUpDownMeans()[i]) / profile->getUpDownStandardDeviations()[i];
  this->upDown /= sample->getUpDownIntervals().size();

  for (std::size_t i = 0; i < sample->getDownUpIntervals().size(); i++)
    this->downUp += (sample->getDownUpIntervals()[i] - profile->getDownUpMeans()[i]) / profile->getDownUpStandardDeviations()[i];
  this->downUp /= sample->getDownUpIntervals().size();
}

// Getters
double Distance::getDownDown() const {
  return this->downDown;
}

double Distance::getUpDown() const {
  return this->upDown;
}

double Distance::getDownUp() const {
  return this->downUp;
}
