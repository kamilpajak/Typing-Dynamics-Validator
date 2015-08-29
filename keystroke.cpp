#include "keystroke.h"

Keystroke::Keystroke(sql::ResultSet* resultSet) {
  this->id = resultSet->getInt("id");
  this->keyCode = resultSet->getInt("keyCode");
  this->keyDownTime = resultSet->getDouble("keyDownTime");
  this->keyUpTime = resultSet->getDouble("keyUpTime");
  this->sampleId = resultSet->getInt("sample_id");
}

// Setters
void Keystroke::setId(int id) {
  this->id = id;
}

void Keystroke::setKeyCode(int keyCode) {
  this->keyCode = keyCode;
}

void Keystroke::setKeyDownTime(double keyDownTime) {
  this->keyDownTime = keyDownTime;
}

void Keystroke::setKeyUpTime(double keyUpTime) {
  this->keyUpTime = keyUpTime;
}

void Keystroke::setSampleId(int sampleId) {
  this->sampleId = sampleId;
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
