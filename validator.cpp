#include "config.h"

#include <iostream>
#include <vector>
#include <string>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

int main() {
  int minimalNumberOfSamples = 10;

  sql::mysql::MySQL_Driver *driver;
  driver = sql::mysql::get_mysql_driver_instance();

  sql::Connection *connection;
  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  sql::PreparedStatement *preparedStatement;
  preparedStatement = connection->prepareStatement("SELECT user_id FROM sample GROUP BY user_id HAVING count(*) > ?;");
  preparedStatement->setInt(1, minimalNumberOfSamples);
  sql::ResultSet *userIDs;
  userIDs = preparedStatement->executeQuery();

  while (userIDs->next()) {
    std::cout << userIDs->getInt("user_id") << std::endl;
  }

  delete userIDs;
  delete preparedStatement;
  delete connection;

  return 0;
}
