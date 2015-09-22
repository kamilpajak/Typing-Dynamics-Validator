#include "config.h"

#include "user.h"
#include "profile.h"
#include "sample.h"
#include "keystroke.h"
#include "classifier.h"

#include <iostream>
#include <algorithm>

#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

enum TimeFeature {
  DOWN_DOWN,
  UP_DOWN,
  DOWN_UP
};

bool isAccepted(Profile* profile, Sample* sample, TimeFeature timeFeature, std::pair<double, double> thresholdParameters) {
  Classifier* classifier = new Classifier(profile, sample);
  switch (timeFeature) {
  case DOWN_DOWN:
    classifier->setDownDownThreshold(profile, thresholdParameters.first, thresholdParameters.second);
    if (classifier->isDownDownValid())
      return true;
    break;
  case UP_DOWN:
    classifier->setUpDownThreshold(profile, thresholdParameters.first, thresholdParameters.second);
    if (classifier->isUpDownValid())
      return true;
    break;
  case DOWN_UP:
    classifier->setDownUpThreshold(profile, thresholdParameters.first, thresholdParameters.second);
    if (classifier->isDownUpValid())
      return true;
    break;
  }

  return false;
}

double calculateFalseAcceptanceRate(std::vector<User*> users, TimeFeature timeFeature, std::pair<double, double> thresholdParameters) {
  int trials = 0;
  int falseAcceptances = 0;

  for (std::size_t i = 0; i < users.size(); i++)
    for (std::size_t j = 0; j < users.size(); j++)
      if (i != j)
        for (Profile* profile : users[i]->getProfiles())
          for (Sample* sample : users[j]->getSamples()) {
            if (isAccepted(profile, sample, timeFeature, thresholdParameters))
              falseAcceptances++;
            trials++;
          }

  if (trials == 0)
    return 0;
  return (double)falseAcceptances / (double)trials;
}

double calculateFalseRejectionRate(std::vector<User*> users, TimeFeature timeFeature, std::pair<double, double> thresholdParameters) {
  int trials = 0;
  int falseRejections = 0;

  for (User* user : users) {
    std::vector<Sample*> samples = user->getSamples();
    std::vector<Profile*> profiles = user->getProfiles();
    if (!profiles.empty())
      for (std::size_t i = 0; i < profiles.size() - 1; i++) {
        Sample* lastSampleOfTrainingSet = profiles[i]->getTrainingSet().back();
        Sample* sample = samples[std::find(samples.begin(), samples.end(), lastSampleOfTrainingSet) - samples.begin() + 1];
        if (!isAccepted(profiles[i], sample, timeFeature, thresholdParameters))
          falseRejections++;
        trials++;
      }
  }

  if (trials == 0)
    return 0;
  return (double)falseRejections / (double)trials;
}

std::pair<double, double> findBestThresholdParameters(std::vector<User*> users, TimeFeature timeFeature) {
  double currentFar;
  double currentFrr;
  double lowestSum = 2;
  std::pair<double, double> bestThresholdParameters;

  double incrementA = -0.5;
  double incrementB = 0.125;

  double minA = -10;
  double maxA = -0.5;
  double minB = 0;
  double maxB = 5;

  std::pair<double, double> thresholdParameters;

  double a = maxA;
  while (a >= minA) {
    double b = minB;
    while (b <= maxB) {
      thresholdParameters.first = a;
      thresholdParameters.second = b;
      currentFar = calculateFalseAcceptanceRate(users, timeFeature, thresholdParameters);
      currentFrr = calculateFalseRejectionRate(users, timeFeature, thresholdParameters);
      if (currentFar + currentFrr < lowestSum) {
        lowestSum = currentFar + currentFrr;
        bestThresholdParameters = thresholdParameters;
      }
      b += incrementB;
    }
    a += incrementA;
  }

  return bestThresholdParameters;
}

int main() {
  sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement* preparedStatement;
  sql::Connection* connection;

  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  preparedStatement = connection->prepareStatement("SELECT * FROM user;");
  sql::ResultSet* fetchedUsers = preparedStatement->executeQuery();
  std::vector<User*> users;
  while (fetchedUsers->next()) {
    preparedStatement = connection->prepareStatement("SELECT * FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, fetchedUsers->getInt("id"));
    sql::ResultSet* fetchedSamples = preparedStatement->executeQuery();
    std::vector<Sample*> samples;
    while (fetchedSamples->next()) {
      preparedStatement = connection->prepareStatement("SELECT * FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, fetchedSamples->getInt("id"));
      sql::ResultSet* fetchedKeystrokes = preparedStatement->executeQuery();
      std::vector<Keystroke*> keystrokes;
      while (fetchedKeystrokes->next()) {
        Keystroke* keystroke = new Keystroke(fetchedKeystrokes);
        keystrokes.push_back(keystroke);
      }
      delete fetchedKeystrokes;
      Sample* sample = new Sample(fetchedSamples, keystrokes);
      samples.push_back(sample);
    }
    delete fetchedSamples;
    User* user = new User(fetchedUsers, samples);
    users.push_back(user);
  }
  delete fetchedUsers;
  delete preparedStatement;
  delete connection;

  findBestThresholdParameters(users, DOWN_DOWN);

  return EXIT_SUCCESS;
}
