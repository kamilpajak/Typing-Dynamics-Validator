#ifndef USER_H
#define USER_H

#include "sample.h"
#include "profile.h"

#include <string>
#include <map>
#include <list>

class User {
private:
  int id;
  std::string username;
  std::map<int, Sample *> samples;
  std::list<Profile *> profiles;

public:
  User(sql::ResultSet *resultSet, std::map<int, Sample *> samples);

  // Setters
  void setId(int id);
  void setUsername(std::string username);

  // Getters
  int getId() const;
  std::string getUsername() const;
};

#endif // USER_H
