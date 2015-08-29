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
};

#endif // PROFILE_H
