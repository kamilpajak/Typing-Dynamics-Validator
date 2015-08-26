#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <vector>
#include <string>

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
