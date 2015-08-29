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
  std::size_t trainingSetSize = 10;

public:
  User(sql::ResultSet* resultSet, std::vector<Sample*> samples);

  // Setters
  void setId(int id);
  void setUsername(std::string username);

  // Getters
  int getId() const;
  std::string getUsername() const;
};

#endif // USER_H
