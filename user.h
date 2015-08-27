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
  std::map<int, Sample> samples;
  std::list<Profile> profiles;

public:
  User();
  void setId(int id);
  int getId() const;
  void setUsername(std::string username);
  std::string getUsername() const;
};

#endif // USER_H
