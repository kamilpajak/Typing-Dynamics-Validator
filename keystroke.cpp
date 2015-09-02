#include "keystroke.h"

Keystroke::Keystroke(sql::ResultSet* resultSet) {
  this->id_ = resultSet->getInt("id");
  this->keyCode_ = resultSet->getInt("keyCode");
  this->keyDownTime_ = resultSet->getDouble("keyDownTime");
  this->keyUpTime_ = resultSet->getDouble("keyUpTime");
  this->sampleId_ = resultSet->getInt("sample_id");
}

// Getters
int Keystroke::getId() const {
  return this->id_;
}

int Keystroke::getKeyCode() const {
  return this->keyCode_;
}

double Keystroke::getKeyDownTime() const {
  return this->keyDownTime_;
}

double Keystroke::getKeyUpTime() const {
  return this->keyUpTime_;
}

int Keystroke::getSampleId() const {
  return this->sampleId_;
}
