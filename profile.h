#ifndef PROFILE_H
#define PROFILE_H

#include "sample.h"

#include <vector>

class Profile {
private:
  std::vector<double> downDownMeans;
  std::vector<double> upDownMeans;
  std::vector<double> downUpMeans;
  std::vector<double> downDownStandardDeviations;
  std::vector<double> upDownStandardDeviations;
  std::vector<double> downUpStandardDeviations;
  std::vector<Sample*> trainingSet;

public:
  Profile(std::vector<Sample*> trainingSet);

  // Getters
  std::vector<double> getDownDownMeans() const;
  std::vector<double> getUpDownMeans() const;
  std::vector<double> getDownUpMeans() const;
  std::vector<double> getDownDownStandardDeviations() const;
  std::vector<double> getUpDownStandardDeviations() const;
  std::vector<double> getDownUpStandardDeviations() const;
};

#endif // PROFILE_H
