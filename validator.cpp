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

double calculateFalseAcceptanceRate(std::vector<User*> users) {
  int trials = 0;
  int falseAcceptances = 0;
  for (std::size_t i = 0; i < users.size(); i++)
    for (std::size_t j = 0; j < users.size(); j++)
      if (i != j)
        for (Profile* profile : users[i]->getProfiles())
          for (Sample* sample : users[j]->getSamples()) {
            Classifier* classifier = new Classifier(profile, sample);
            if (classifier->isValid())
              falseAcceptances++;
            trials++;
          }

  std::cout << falseAcceptances << " / " << trials << " => " << (double)falseAcceptances / (double)trials * 100 << "% FAR" << std::endl;
  return (double)falseAcceptances / (double)trials;
}

double calculateFalseRejectionRate(std::vector<User*> users) {
  int trials = 0;
  int falseRejections = 0;
  for (User* user : users) {
    std::vector<Sample*> samples = user->getSamples();
    std::vector<Profile*> profiles = user->getProfiles();
    if (!profiles.empty())
      for (std::size_t i = 0; i < profiles.size() - 1; i++) {
        Sample* lastSampleOfTrainingSet = profiles[i]->getTrainingSet().back();
        Sample* sample = samples[std::find(samples.begin(), samples.end(), lastSampleOfTrainingSet) - samples.begin() + 1];
        Classifier* classifier = new Classifier(profiles[i], sample);
        if (!classifier->isValid())
          falseRejections++;
        trials++;
      }
  }

  std::cout << falseRejections << " / " << trials << " => " << (double)falseRejections / (double)trials * 100 << "% FRR" << std::endl;
  return (double)falseRejections / (double)trials;
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

  // False acceptance and false rejection rates
  double lowestFalseAcceptanceRate = 0;
  double lowestFalseRejectionRate = 0;

  calculateFalseAcceptanceRate(users);
  calculateFalseRejectionRate(users);

  return EXIT_SUCCESS;
}
