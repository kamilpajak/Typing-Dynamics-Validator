#ifndef KEYSTROKE_H
#define KEYSTROKE_H

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
