#include "sample.h"

Sample::Sample(sql::ResultSet* resultSet, std::vector<Keystroke*> keystrokes) {
  this->id = resultSet->getInt("id");
  this->inputDevice = resultSet->getString("input_device");
  this->keystrokes = keystrokes;
  this->userId = resultSet->getInt("user_id");
  this->calculateDownDownIntervals();
  this->calculateUpDownIntervals();
  this->calculateDownUpIntervals();
}

void Sample::calculateDownDownIntervals() {
  for (std::size_t i = 0; i < this->keystrokes.size() - 1; i++)
    this->downDownIntervals.push_back(this->keystrokes[i + 1]->getKeyDownTime() - this->keystrokes[i]->getKeyDownTime());
}

void Sample::calculateUpDownIntervals() {
  for (std::size_t i = 0; i < this->keystrokes.size() - 1; i++)
    this->upDownIntervals.push_back(this->keystrokes[i + 1]->getKeyDownTime() - this->keystrokes[i]->getKeyUpTime());
}

void Sample::calculateDownUpIntervals() {
  for (Keystroke* keystroke : this->keystrokes)
    this->downUpIntervals.push_back(keystroke->getKeyUpTime() - keystroke->getKeyDownTime());
}

// Getters
std::vector<double> Sample::getDownDownIntervals() const {
  return this->downDownIntervals;
}

std::vector<double> Sample::getUpDownIntervals() const {
  return this->upDownIntervals;
}

std::vector<double> Sample::getDownUpIntervals() const {
  return this->downUpIntervals;
}
