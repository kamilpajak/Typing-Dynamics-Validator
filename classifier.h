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

public:
  Classifier(Profile* profile, Sample* sample);
  void setDownDownThreshold(Profile* profile, double a, double b);
  void setUpDownThreshold(Profile* profile, double a, double b);
  void setDownUpThreshold(Profile* profile, double a, double b);
  double getDownDownDistance() const;
  double getUpDownDistance() const;
  double getDownUpDistance() const;
  double getDownDownThreshold() const;
  double getUpDownThreshold() const;
  double getDownUpThreshold() const;
  bool isValid();
  bool isDownDownValid();
  bool isUpDownValid();
  bool isDownUpValid();
};

#endif // CLASSIFIER_H
