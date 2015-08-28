#include "user.h"

User::User(sql::ResultSet *resultSet, std::map<int, Sample *> samples) {
  this->id = resultSet->getInt("id");
  this->username = resultSet->getString("username");
  this->samples = samples;
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
