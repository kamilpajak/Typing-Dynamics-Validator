#ifndef KEYSTROKE_H
#define KEYSTROKE_H

#include <cppconn/resultset.h>

class Keystroke {
private:
  int id;
  int keyCode;
  double keyDownTime;
  double keyUpTime;
  int sampleId;

public:
  Keystroke(sql::ResultSet *resultSet);

  // Setters
  void setId(int id);
  void setKeyCode(int keyCode);
  void setKeyDownTime(double keyDownTime);
  void setKeyUpTime(double keyUpTime);
  void setSampleId(int sampleId);

  // Getters
  int getId() const;
  int getKeyCode() const;
  double getKeyDownTime() const;
  double getKeyUpTime() const;
  int getSampleId() const;
};

#endif // KEYSTROKE_H
