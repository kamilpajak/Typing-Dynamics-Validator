#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "profile.h"
#include "sample.h"

class Classifier {
private:
  double downDownDistance;
  double upDownDistance;
  double downUpDistance;
  double downDownThreshold;
  double upDownThreshold;
  double downUpThreshold;
  bool isValid;

public:
  Classifier(Profile* profile, Sample* sample);
  double getDownDownDistance() const;
  double getUpDownDistance() const;
  double getDownUpDistance() const;
  double getDownDownThreshold() const;
  double getUpDownThreshold() const;
  double getDownUpThreshold() const;
  bool isValidSample() const;
};

#endif // CLASSIFIER_H
