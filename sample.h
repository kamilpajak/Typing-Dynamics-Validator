#ifndef SAMPLE_H
#define SAMPLE_H

#include "keystroke.h"

#include <string>
#include <vector>

#include <cppconn/resultset.h>

class Sample {
private:
  int id_;
  std::string inputDevice_;
  int userId_;
  std::vector<double> downDownIntervals_;
  std::vector<double> upDownIntervals_;
  std::vector<double> downUpIntervals_;
  std::vector<Keystroke*> keystrokes_;

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
