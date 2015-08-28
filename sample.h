#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <string>
#include <map>

class Sample {
private:
  int id;
  std::string inputDevice;
  std::map<int, Keystroke> keystrokes;
  int userId;

public:
  Sample();
  void setId(int id);
  int getId() const;
  void setInputDevice(std::string inputDevice);
  std::string getInputDevice() const;

};

#endif // SAMPLE_H
