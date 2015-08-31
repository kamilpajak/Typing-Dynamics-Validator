#ifndef USER_H
#define USER_H

#include "sample.h"
#include "profile.h"

#include <string>
#include <vector>

class User {
private:
  int id;
  std::string username;
  std::vector<Sample*> samples;
  std::vector<Profile*> profiles;
  std::size_t trainingSetSize;

public:
  User(sql::ResultSet* resultSet, std::vector<Sample*> samples);

  // Getters
  int getId() const;
  std::string getUsername() const;
  std::vector<Sample*> getSamples() const;
  std::vector<Profile*> getProfiles() const;
};

#endif // USER_H
