#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <string>
#include <vector>

class Sample {
private:
  int id;
  int userID;
  std::string inputDevice;
  std::vector<Keystroke> keystrokes;

public:
  Sample();
};

#endif // SAMPLE_H
