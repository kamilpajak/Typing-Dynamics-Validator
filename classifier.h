#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "profile.h"
#include "sample.h"

class Classifier {
private:
  double downDownDistance_;
  double upDownDistance_;
  double downUpDistance_;
  double downDownThreshold_;
  double upDownThreshold_;
  double downUpThreshold_;
  bool isValid_;

public:
  Classifier(Profile* profile, Sample* sample);
  double getDownDownDistance() const;
  double getUpDownDistance() const;
  double getDownUpDistance() const;
  double getDownDownThreshold() const;
  double getUpDownThreshold() const;
  double getDownUpThreshold() const;
  bool isValid() const;
};

#endif // CLASSIFIER_H
