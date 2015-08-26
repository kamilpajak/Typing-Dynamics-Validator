#ifndef KEYSTROKE_H
#define KEYSTROKE_H

#include <vector>
#include <string>

class Keystroke {
private:
  int id;
  int sampleID;
  int keyCode;
  double keyDownTime;
  double keyUpTime;

public:
  Keystroke();
};

#endif // KEYSTROKE_H
