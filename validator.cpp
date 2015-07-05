#include "config.h"

#include <iostream>
#include <vector>
#include <string>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

struct Keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

struct SummaryOfSampleCharacteristics {
  int userID;
  int sampleID;
  std::vector<double> downDownTimes;
  std::vector<double> upDownTimes;
  std::vector<double> downUpTimes;
};

struct Profile {
  std::vector<double> downDownMeans;
  std::vector<double> upDownMeans;
  std::vector<double> downUpMeans;
  std::vector<double> downDownStandardDeviations;
  std::vector<double> upDownStandardDeviations;
  std::vector<double> downUpStandardDeviations;
};

std::vector<double> takeDownDownTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> downDownTimes;
  for (unsigned int i = 0; i < keystrokes.size() - 1; i++) {
    double downDownTime =
        keystrokes[i + 1].keyDownTime - keystrokes[i].keyDownTime;
    downDownTimes.push_back(downDownTime);
  }

  return downDownTimes;
}

std::vector<double> takeUpDownTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> upDownTimes;
  for (unsigned int i = 0; i < keystrokes.size() - 1; i++) {
    double upDownTime = keystrokes[i + 1].keyDownTime - keystrokes[i].keyUpTime;
    upDownTimes.push_back(upDownTime);
  }

  return upDownTimes;
}

std::vector<double> takeDownUpTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> downUpTimes;
  for (unsigned int i = 0; i < keystrokes.size(); i++) {
    double downUpTime = keystrokes[i].keyUpTime - keystrokes[i].keyDownTime;
    downUpTimes.push_back(downUpTime);
  }

  return downUpTimes;
}

Profile takeProfile(std::vector<SummaryOfSampleCharacteristics> inputSamples) {
  Profile profile;
}

int main() {
  int minimalNumberOfSamples = 10;
  int samplesPerProfile = 10;
  std::vector<SummaryOfSampleCharacteristics> samples;
  std::vector<Profile> profiles;
  // ------------------------------------------------------------------------ //
  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement *preparedStatement;
  sql::Connection *connection;

  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  preparedStatement = connection->prepareStatement(
      "SELECT user_id FROM sample GROUP BY user_id HAVING count(*) > ?;");
  preparedStatement->setInt(1, minimalNumberOfSamples);
  sql::ResultSet *userIDs = preparedStatement->executeQuery();

  while (userIDs->next()) {
    preparedStatement =
        connection->prepareStatement("SELECT id FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, userIDs->getInt("user_id"));
    sql::ResultSet *sampleIDs = preparedStatement->executeQuery();

    while (sampleIDs->next()) {
      preparedStatement =
          connection->prepareStatement("SELECT keyCode, keyDownTime, keyUpTime "
                                       "FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, sampleIDs->getInt("id"));
      sql::ResultSet *keystrokeSQL = preparedStatement->executeQuery();

      std::vector<Keystroke> keystrokes;
      while (keystrokeSQL->next()) {
        Keystroke keystroke;
        keystroke.keyCode = keystrokeSQL->getInt("keyCode");
        keystroke.keyDownTime = keystrokeSQL->getDouble("keyDownTime");
        keystroke.keyUpTime = keystrokeSQL->getDouble("keyUpTime");
        keystrokes.push_back(keystroke);
      }

      delete keystrokeSQL;
      SummaryOfSampleCharacteristics summaryOfSample;
      summaryOfSample.userID = userIDs->getInt("user_id");
      summaryOfSample.sampleID = sampleIDs->getInt("id");
      summaryOfSample.downDownTimes = takeDownDownTimes(keystrokes);
      summaryOfSample.upDownTimes = takeUpDownTimes(keystrokes);
      summaryOfSample.downUpTimes = takeDownUpTimes(keystrokes);
      samples.push_back(summaryOfSample);
    }

    delete sampleIDs;
  }

  delete userIDs;
  delete preparedStatement;
  delete connection;
  // ------------------------------------------------------------------------ //
  for (unsigned int i = 0; i < samples.size() - samplesPerProfile + 1; i++) {
    if (samples[i].userID == samples[i + samplesPerProfile - 1].userID) {
    }
  }

  return 0;
}
