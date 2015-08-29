#ifndef DISTANCE_H
#define DISTANCE_H

#include "profile.h"
#include "sample.h"

class Distance {
private:
  double downDown;
  double upDown;
  double downUp;

public:
  Distance(Profile* profile, Sample* sample);

  // Getters
  double getDownDown() const;
  double getUpDown() const;
  double getDownUp() const;
};

#endif // DISTANCE_H
