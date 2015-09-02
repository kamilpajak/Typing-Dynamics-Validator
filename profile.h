#ifndef PROFILE_H
#define PROFILE_H

#include "sample.h"

#include <vector>

class Profile {
private:
  std::vector<double> downDownMeans_;
  std::vector<double> upDownMeans_;
  std::vector<double> downUpMeans_;
  std::vector<double> downDownStandardDeviations_;
  std::vector<double> upDownStandardDeviations_;
  std::vector<double> downUpStandardDeviations_;
  double downDownMeanDeviation_;
  double upDownMeanDeviation_;
  double downUpMeanDeviation_;
  std::vector<Sample*> trainingSet_;

public:
  Profile(std::vector<Sample*> trainingSet);

  // Getters
  std::vector<double> getDownDownMeans() const;
  std::vector<double> getUpDownMeans() const;
  std::vector<double> getDownUpMeans() const;
  std::vector<double> getDownDownStandardDeviations() const;
  std::vector<double> getUpDownStandardDeviations() const;
  std::vector<double> getDownUpStandardDeviations() const;
  double getDownDownMeanDeviation() const;
  double getUpDownMeanDeviation() const;
  double getDownUpMeanDeviation() const;
  std::vector<Sample*> getTrainingSet() const;
};

#endif // PROFILE_H
