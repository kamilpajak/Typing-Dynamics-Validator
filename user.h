#ifndef USER_H
#define USER_H

#include "sample.h"
#include "profile.h"

#include <vector>
#include <string>

class User {
private:
  int id;
  std::string username;
  std::vector<Sample> samples;
  std::vector<Profile> profiles;

public:
  User();
};

#endif // USER_H
