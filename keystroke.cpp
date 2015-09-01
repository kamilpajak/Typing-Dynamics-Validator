#include "keystroke.h"

Keystroke::Keystroke(sql::ResultSet* resultSet) {
  this->id = resultSet->getInt("id");
  this->keyCode = resultSet->getInt("keyCode");
  this->keyDownTime = resultSet->getDouble("keyDownTime");
  this->keyUpTime = resultSet->getDouble("keyUpTime");
  this->sampleId = resultSet->getInt("sample_id");
}

// Getters
int Keystroke::getId() const {
  return this->id;
}

int Keystroke::getKeyCode() const {
  return this->keyCode;
}

double Keystroke::getKeyDownTime() const {
  return this->keyDownTime;
}

double Keystroke::getKeyUpTime() const {
  return this->keyUpTime;
}

int Keystroke::getSampleId() const {
  return this->sampleId;
}
