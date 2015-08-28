#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <string>
#include <map>

#include <cppconn/resultset.h>

class Sample {
private:
  int id;
  std::string inputDevice;
  std::map<int, Keystroke *> keystrokes;
  int userId;

public:
  Sample(sql::ResultSet *resultSet, std::map<int, Keystroke *> keystrokes);

  // Setters
  void setId(int id);
  void setInputDevice(std::string inputDevice);

  // Getters
  int getId() const;
  std::string getInputDevice() const;
};

#endif // SAMPLE_H
