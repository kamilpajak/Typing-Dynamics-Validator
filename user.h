#ifndef USER_H
#define USER_H

#include "sample.h"
#include "profile.h"

#include <string>
#include <vector>

class User {
private:
  int id_;
  std::string username_;
  std::vector<Sample*> samples_;
  std::vector<Profile*> profiles_;
  std::size_t trainingSetSize_;

public:
  User(sql::ResultSet* resultSet, std::vector<Sample*> samples);

  // Getters
  int getId() const;
  std::string getUsername() const;
  std::vector<Sample*> getSamples() const;
  std::vector<Profile*> getProfiles() const;
};

#endif // USER_H
