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
  int userId;
  std::vector<double> downDownIntervals;
  std::vector<double> upDownIntervals;
  std::vector<double> downUpIntervals;
  std::vector<Keystroke*> keystrokes;

  void calculateDownDownIntervals();
  void calculateUpDownIntervals();
  void calculateDownUpIntervals();

public:
  Sample(sql::ResultSet* resultSet, std::vector<Keystroke*> keystrokes);

  // Getters
  int getId() const;
  std::vector<double> getDownDownIntervals() const;
  std::vector<double> getUpDownIntervals() const;
  std::vector<double> getDownUpIntervals() const;
};

#endif // SAMPLE_H
