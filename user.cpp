#include "user.h"

User::User(sql::ResultSet* resultSet, std::vector<Sample*> samples) {
  this->id = resultSet->getInt("id");
  this->username = resultSet->getString("username");
  this->samples = samples;

  if (this->samples.size() >= this->trainingSetSize) {
    std::vector<Sample*> trainingSet;
    for (std::size_t i = 0; i <= this->samples.size() - this->trainingSetSize; i++) {
      trainingSet.clear();
      for (std::size_t j = i; j < i + this->trainingSetSize; j++)
        trainingSet.push_back(this->samples[j]);
      Profile* profile = new Profile(trainingSet);
      this->profiles.push_back(profile);
    }
  }
}

// Setters
void User::setId(int id) {
  this->id = id;
}

int User::getId() const {
  return this->id;
}

// Getters
void User::setUsername(std::string username) {
  this->username = username;
}

std::string User::getUsername() const {
  return this->username;
}
