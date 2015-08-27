#ifndef KEYSTROKE_H
#define KEYSTROKE_H

class Keystroke {
private:
  int id;
  int sampleId;
  int keyCode;
  double keyDownTime;
  double keyUpTime;

public:
  Keystroke();
  void setId(int id);
  int getId() const;
  void setSampleId(int sampleId);
  int getSampleId() const;
  void setKeyCode(int keyCode);
  int getKeyCode() const;
  void setKeyDownTime(double keyDownTime);
  double getKeyDownTime() const;
  void setKeyUpTime(double keyUpTime);
  double getKeyUpTime() const;
};

#endif // KEYSTROKE_H
