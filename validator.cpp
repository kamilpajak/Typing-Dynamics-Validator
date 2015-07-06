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
  int userID_SQL;
  int sampleID_SQL;
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
  std::vector<double> downDownMeans;
  std::vector<double> upDownMeans;
  std::vector<double> downUpMeans;

  std::vector<double> downDownStandardDeviations;
  std::vector<double> upDownStandardDeviations;
  std::vector<double> downUpStandardDeviations;

  double counter;

  // Means
  for (unsigned int i = 0; i < inputSamples[0].downDownTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += inputSamples[j].downDownTimes[i];
    downDownMeans.push_back(counter / inputSamples.size());
  }

  for (unsigned int i = 0; i < inputSamples[0].upDownTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += inputSamples[j].upDownTimes[i];
    upDownMeans.push_back(counter / inputSamples.size());
  }

  for (unsigned int i = 0; i < inputSamples[0].downUpTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += inputSamples[j].downUpTimes[i];
    downUpMeans.push_back(counter / inputSamples.size());
  }

  // Standard deviations
  for (unsigned int i = 0; i < inputSamples[0].downDownTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += abs(inputSamples[j].downDownTimes[i] - downDownMeans[i]);
    downDownStandardDeviations.push_back(counter / (inputSamples.size() - 1));
  }

  for (unsigned int i = 0; i < inputSamples[0].upDownTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += abs(inputSamples[j].upDownTimes[i] - upDownMeans[i]);
    upDownStandardDeviations.push_back(counter / (inputSamples.size() - 1));
  }

  for (unsigned int i = 0; i < inputSamples[0].downUpTimes.size(); i++) {
    counter = 0;
    for (unsigned int j = 0; j < inputSamples.size(); j++)
      counter += abs(inputSamples[j].downUpTimes[i] - downUpMeans[i]);
    downUpStandardDeviations.push_back(counter / (inputSamples.size() - 1));
  }

  Profile profile;
  profile.downDownMeans = downDownMeans;
  profile.upDownMeans = upDownMeans;
  profile.downUpMeans = downUpMeans;
  profile.downDownStandardDeviations = downDownStandardDeviations;
  profile.upDownStandardDeviations = upDownStandardDeviations;
  profile.downUpStandardDeviations = downUpStandardDeviations;

  return profile;
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
      summaryOfSample.userID_SQL = userIDs->getInt("user_id");
      summaryOfSample.sampleID_SQL= sampleIDs->getInt("id");
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

  return 0;
}
