#include "sample.h"

Sample::Sample(sql::ResultSet *resultSet, std::map<int, Keystroke *> keystrokes) {
  this->id = resultSet->getInt("id");
  this->inputDevice = resultSet->getString("input_device");
  this->keystrokes = keystrokes;
  this->userId = resultSet->getInt("user_id");
}
