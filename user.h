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
};

#endif // USER_H
