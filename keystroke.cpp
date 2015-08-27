#include "keystroke.h"

Keystroke::Keystroke() {
}

void Keystroke::setId(int id) {
  this->id = id;
}

int Keystroke::getId() const {
  return this->id;
}

void Keystroke::setSampleId(int sampleId) {
  this->sampleId = sampleId;
}

int Keystroke::getSampleId() const {
  return this->sampleId;
}
void Keystroke::setKeyCode(int keyCode) {
  this->keyCode = keyCode;
}

int Keystroke::getKeyCode() const {
  return this->keyCode;
}
void Keystroke::setKeyDownTime(double keyDownTime) {
  this->keyDownTime = keyDownTime;
}

double Keystroke::getKeyDownTime() const {
  return this->keyDownTime;
}

void Keystroke::setKeyUpTime(double keyUpTime) {
  this->keyUpTime = keyUpTime;
}

double Keystroke::getKeyUpTime() const {
  return this->keyUpTime;
}
