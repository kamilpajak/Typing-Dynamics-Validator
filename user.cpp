#include "user.h"

User::User() {
}

void User::setId(int id) {
  this->id = id;
}

int User::getId() const {
  return this->id;
}

void User::setUsername(std::string username) {
  this->username = username;
}

std::string User::getUsername() const {
  return this->username;
}
