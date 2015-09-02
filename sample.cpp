#include "sample.h"

Sample::Sample(sql::ResultSet* resultSet, std::vector<Keystroke*> keystrokes) {
  this->id_ = resultSet->getInt("id");
  this->inputDevice_ = resultSet->getString("input_device");
  this->keystrokes_ = keystrokes;
  this->userId_ = resultSet->getInt("user_id");
  this->calculateDownDownIntervals();
  this->calculateUpDownIntervals();
  this->calculateDownUpIntervals();
}

void Sample::calculateDownDownIntervals() {
  for (std::size_t i = 0; i < this->keystrokes_.size() - 1; i++)
    this->downDownIntervals_.push_back(this->keystrokes_[i + 1]->getKeyDownTime() - this->keystrokes_[i]->getKeyDownTime());
}

void Sample::calculateUpDownIntervals() {
  for (std::size_t i = 0; i < this->keystrokes_.size() - 1; i++)
    this->upDownIntervals_.push_back(this->keystrokes_[i + 1]->getKeyDownTime() - this->keystrokes_[i]->getKeyUpTime());
}

void Sample::calculateDownUpIntervals() {
  for (Keystroke* keystroke : this->keystrokes_)
    this->downUpIntervals_.push_back(keystroke->getKeyUpTime() - keystroke->getKeyDownTime());
}

// Getters
int Sample::getId() const {
  return this->id_;
}

std::vector<double> Sample::getDownDownIntervals() const {
  return this->downDownIntervals_;
}

std::vector<double> Sample::getUpDownIntervals() const {
  return this->upDownIntervals_;
}

std::vector<double> Sample::getDownUpIntervals() const {
  return this->downUpIntervals_;
}
