#include "config.h"

#include "user.h"
#include "sample.h"
#include "keystroke.h"
#include "profile.h"

#include <iostream>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

int main() {
  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement *preparedStatement;
  sql::Connection *connection;

  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  preparedStatement = connection->prepareStatement("SELECT * FROM user;");
  sql::ResultSet *fetchedUsers = preparedStatement->executeQuery();
  std::map<int, User *> users;
  while (fetchedUsers->next()) {
    preparedStatement = connection->prepareStatement("SELECT * FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, fetchedUsers->getInt("id"));
    sql::ResultSet *fetchedSamples = preparedStatement->executeQuery();
    std::map<int, Sample *> samples;
    while (fetchedSamples->next()) {
      preparedStatement = connection->prepareStatement("SELECT * FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, fetchedSamples->getInt("id"));
      sql::ResultSet *fetchedKeystrokes = preparedStatement->executeQuery();
      std::map<int, Keystroke *> keystrokes;
      while (fetchedKeystrokes->next()) {
        Keystroke *keystroke = new Keystroke(fetchedKeystrokes);
        keystrokes[fetchedKeystrokes->getInt("id")] = keystroke;
      }
      delete fetchedKeystrokes;
      Sample *sample = new Sample(fetchedSamples, keystrokes);
      samples[fetchedSamples->getInt("id")] = sample;
    }
    delete fetchedSamples;
    User *user = new User(fetchedUsers, samples);
    users[fetchedUsers->getInt("id")] = user;
  }
  delete fetchedUsers;
  delete preparedStatement;
  delete connection;
  return 0;
}
