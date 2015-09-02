#include "user.h"

User::User(sql::ResultSet* resultSet, std::vector<Sample*> samples) {
  this->trainingSetSize_ = 10;
  this->id_ = resultSet->getInt("id");
  this->username_ = resultSet->getString("username");
  this->samples_ = samples;

  if (this->samples_.size() >= this->trainingSetSize_) {
    std::vector<Sample*> trainingSet;
    for (std::size_t i = 0; i <= this->samples_.size() - this->trainingSetSize_; i++) {
      trainingSet.clear();
      for (std::size_t j = i; j < i + this->trainingSetSize_; j++)
        trainingSet.push_back(this->samples_[j]);
      Profile* profile = new Profile(trainingSet);
      this->profiles_.push_back(profile);
    }
  }
}

// Getters
int User::getId() const {
  return this->id_;
}

std::string User::getUsername() const {
  return this->username_;
}

std::vector<Sample*> User::getSamples() const {
  return this->samples_;
}

std::vector<Profile*> User::getProfiles() const {
  return this->profiles_;
}
