#ifndef KEYSTROKE_H
#define KEYSTROKE_H

#include <cppconn/resultset.h>

class Keystroke {
private:
  int id_;
  int keyCode_;
  double keyDownTime_;
  double keyUpTime_;
  int sampleId_;

public:
  Keystroke(sql::ResultSet* resultSet);

  // Getters
  int getId() const;
  int getKeyCode() const;
  double getKeyDownTime() const;
  double getKeyUpTime() const;
  int getSampleId() const;
};

#endif // KEYSTROKE_H
