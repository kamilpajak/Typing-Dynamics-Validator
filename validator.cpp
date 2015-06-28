#include "config.h"

#include <iostream>
#include <vector>
#include <string>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

struct sampleFeatures {
  std::vector<int> downDownTime;
  std::vector<int> upDownTime;
  std::vector<int> downUpTime;
};

int main() {
  int usersMinimalNumberOfSamples = 10;

  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement *preparedStatement;
  sql::Connection *connection;

  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  preparedStatement = connection->prepareStatement(
      "SELECT user_id FROM sample GROUP BY user_id HAVING count(*) > ?;");
  preparedStatement->setInt(1, usersMinimalNumberOfSamples);
  sql::ResultSet *userIDs = preparedStatement->executeQuery();

  while (userIDs->next()) {
    preparedStatement =
        connection->prepareStatement("SELECT id FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, userIDs->getInt("user_id"));
    sql::ResultSet *sampleIDs = preparedStatement->executeQuery();

    while (sampleIDs->next()) {
      preparedStatement = connection->prepareStatement(
          "SELECT keyDownTime, keyUpTime FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, sampleIDs->getInt("id"));
      sql::ResultSet *keystrokeData = preparedStatement->executeQuery();

      while (keystrokeData->next()) {
      }

      delete keystrokeData;
    }

    delete sampleIDs;
  }

  delete userIDs;
  delete preparedStatement;
  delete connection;

  return 0;
}
