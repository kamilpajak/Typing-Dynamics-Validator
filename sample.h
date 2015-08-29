#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <string>
#include <vector>

#include <cppconn/resultset.h>

class Sample {
private:
  int id;
  std::string inputDevice;
  std::vector<Keystroke*> keystrokes;
  int userId;
  std::vector<double> downDownIntervals;
  std::vector<double> upDownIntervals;
  std::vector<double> downUpIntervals;

  void calculateDownDownIntervals();
  void calculateUpDownIntervals();
  void calculateDownUpIntervals();

public:
  Sample(sql::ResultSet* resultSet, std::vector<Keystroke*> keystrokes);

  // Getters
  std::vector<double> getDownDownIntervals() const;
  std::vector<double> getUpDownIntervals() const;
  std::vector<double> getDownUpIntervals() const;
};

#endif // SAMPLE_H
