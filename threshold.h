#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "profile.h"

class Threshold {
private:
  double downDown;
  double upDown;
  double downUp;

public:
  Threshold(Profile* profile);

  // Getters
  double getDownDown() const;
  double getUpDown() const;
  double getDownUp() const;
};

#endif // THRESHOLD_H
