#include "config.h"

#include "user.h"
#include "profile.h"
#include "sample.h"
#include "keystroke.h"
#include "classifier.h"

#include <iostream>
#include <algorithm>
#include <fstream>

#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

enum TimeFeature {
  DOWN_DOWN,
  UP_DOWN,
  DOWN_UP
};

struct Comparison {
  TimeFeature timeFeature;
  bool isLegitimate;
  double distance;
  double featureMeanDeviation;
};

std::vector<Comparison> getLegitimatePoints(std::vector<User*> users, TimeFeature timeFeature) {
  std::vector<Comparison> comparisons;

  for (User* user : users) {
    std::vector<Sample*> samples = user->getSamples();
    std::vector<Profile*> profiles = user->getProfiles();
    if (!profiles.empty())
      for (std::size_t i = 0; i < profiles.size() - 1; i++) {
        Sample* lastSampleOfTrainingSet = profiles[i]->getTrainingSet().back();
        Sample* sample = samples[std::find(samples.begin(), samples.end(), lastSampleOfTrainingSet) - samples.begin() + 1];
        Classifier* classifier = new Classifier(profiles[i], sample);
        Comparison comparison;
        comparison.timeFeature = timeFeature;
        comparison.isLegitimate = true;
        switch (timeFeature) {
        case DOWN_DOWN:
          comparison.distance = classifier->getDownDownDistance();
          comparison.featureMeanDeviation = profiles[i]->getDownDownMeanDeviation();
          break;
        case UP_DOWN:
          comparison.distance = classifier->getUpDownDistance();
          comparison.featureMeanDeviation = profiles[i]->getUpDownMeanDeviation();
          break;
        case DOWN_UP:
          comparison.distance = classifier->getDownUpDistance();
          comparison.featureMeanDeviation = profiles[i]->getDownUpMeanDeviation();
          break;
        }
        comparisons.push_back(comparison);
      }
  }

  return comparisons;
}

std::vector<Comparison> getImpostorPoints(std::vector<User*> users, TimeFeature timeFeature) {
  std::vector<Comparison> comparisons;
  for (std::size_t i = 0; i < users.size(); i++)
    for (std::size_t j = 0; j < users.size(); j++)
      if (i != j)
        for (Profile* profile : users[i]->getProfiles())
          for (Sample* sample : users[j]->getSamples()) {
            Classifier* classifier = new Classifier(profile, sample);
            Comparison comparison;
            comparison.timeFeature = timeFeature;
            comparison.isLegitimate = false;
            switch (timeFeature) {
            case DOWN_DOWN:
              comparison.distance = classifier->getDownDownDistance();
              comparison.featureMeanDeviation = profile->getDownDownMeanDeviation();
              break;
            case UP_DOWN:
              comparison.distance = classifier->getUpDownDistance();
              comparison.featureMeanDeviation = profile->getUpDownMeanDeviation();
              break;
            case DOWN_UP:
              comparison.distance = classifier->getDownUpDistance();
              comparison.featureMeanDeviation = profile->getDownUpMeanDeviation();
              break;
            }
            comparisons.push_back(comparison);
          }

  return comparisons;
}

std::vector<Comparison> getPoints(std::vector<User*> users, TimeFeature timeFeature) {
  std::vector<Comparison> legitimatePoints = getLegitimatePoints(users, timeFeature);
  std::vector<Comparison> impostorPoints = getImpostorPoints(users, timeFeature);

  std::vector<Comparison> comparisons;
  comparisons.insert(std::end(comparisons), std::begin(impostorPoints), std::end(impostorPoints));
  comparisons.insert(std::end(comparisons), std::begin(legitimatePoints), std::end(legitimatePoints));

  return comparisons;
}

std::pair<double, double> findBestThresholdParameters(std::vector<Comparison> comparisons) {
  const double incrementA = -0.5;
  const double incrementB = 0.125;
  const double minA = -10;
  const double maxA = -0.5;
  const double minB = 0;
  const double maxB = 5;

  int falseAcceptances;
  int falseRejections;
  double currentFar;
  double currentFrr;
  double lowestSum = 2;
  std::pair<double, double> bestThresholdParameters;

  int legitimateUserAttempts = 0;
  int impostorAttempts = 0;
  for (Comparison comparison : comparisons) {
    if (comparison.isLegitimate)
      legitimateUserAttempts++;
    else
      impostorAttempts++;
  }

  double a = maxA;
  while (a >= minA) {
    double b = minB;
    while (b <= maxB) {
      falseAcceptances = 0;
      falseRejections = 0;
      for (Comparison comparison : comparisons) {
        double threshold = a * comparison.featureMeanDeviation + b;
        if (comparison.distance <= threshold && comparison.isLegitimate == false)
          falseAcceptances++;
        if (comparison.distance > threshold && comparison.isLegitimate == true)
          falseRejections++;
      }

      currentFar = (double)falseAcceptances / (double)impostorAttempts;
      currentFrr = (double)falseRejections / (double)legitimateUserAttempts;
      if (currentFar + currentFrr < lowestSum) {
        lowestSum = currentFar + currentFrr;
        bestThresholdParameters = std::make_pair(a, b);
      }

      b += incrementB;
    }

    a += incrementA;
  }

  falseAcceptances = 0;
  falseRejections = 0;

  for (Comparison comparison : comparisons) {
    double threshold = bestThresholdParameters.first * comparison.featureMeanDeviation + bestThresholdParameters.second;
    if (comparison.distance <= threshold && comparison.isLegitimate == false)
      falseAcceptances++;
    if (comparison.distance > threshold && comparison.isLegitimate == true)
      falseRejections++;
  }

  double FAR = (double)falseAcceptances / (double)impostorAttempts * 100;
  double FRR = (double)falseRejections / (double)legitimateUserAttempts * 100;

  std::cout << "  False Acceptance Rate: " << FAR << "%\n";
  std::cout << "  False Rejection Rate: " << FRR << "%\n";

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

  std::vector<Comparison> downDownComparisons = getPoints(users, DOWN_DOWN);
  std::vector<Comparison> upDownComparisons = getPoints(users, UP_DOWN);
  std::vector<Comparison> downUpComparisons = getPoints(users, DOWN_UP);

  std::cout << "Down Down Error Rates:\n";
  std::pair<double, double> bestDownDownParameters = findBestThresholdParameters(downDownComparisons);
  std::cout << "Up Down Error Rates:\n";
  std::pair<double, double> bestUpDownParameters = findBestThresholdParameters(upDownComparisons);
  std::cout << "Down Up Error Rates:\n";
  std::pair<double, double> bestDownUpParameters = findBestThresholdParameters(downUpComparisons);

  // Calculate legiamate user and impostor attempts among all comparisons
  int legitimateUserAttempts = 0;
  int impostorAttempts = 0;
  for (Comparison downDownComparison : downDownComparisons) {
    if (downDownComparison.isLegitimate)
      legitimateUserAttempts++;
    else
      impostorAttempts++;
  }

  for (Comparison upDownComparison : upDownComparisons) {
    if (upDownComparison.isLegitimate)
      legitimateUserAttempts++;
    else
      impostorAttempts++;
  }

  for (Comparison downUpComparison : downUpComparisons) {
    if (downUpComparison.isLegitimate)
      legitimateUserAttempts++;
    else
      impostorAttempts++;
  }

  int falseAcceptances = 0;
  int falseRejections = 0;

  remove("downDownSeparation.csv");
  remove("upDownSeparation.csv");
  remove("downUpSeparation.csv");

  std::ofstream file;

  file.open("downDownSeparation.csv", std::ios::out | std::ios::app);
  file << "Distance,FeatureMeanDeviation,Legitimate\n";
  for (Comparison downDownComparison : downDownComparisons) {
    double threshold = bestDownDownParameters.first * downDownComparison.featureMeanDeviation + bestDownDownParameters.second;
    if (downDownComparison.distance <= threshold && downDownComparison.isLegitimate == false)
      falseAcceptances++;
    if (downDownComparison.distance > threshold && downDownComparison.isLegitimate == true)
      falseRejections++;

    file << downDownComparison.distance << "," << downDownComparison.featureMeanDeviation << "," << downDownComparison.isLegitimate << "\n";
  }
  file.close();

  file.open("upDownSeparation.csv", std::ios::out | std::ios::app);
  file << "Distance,FeatureMeanDeviation,Legitimate\n";
  for (Comparison upDownComparison : upDownComparisons) {
    double threshold = bestUpDownParameters.first * upDownComparison.featureMeanDeviation + bestUpDownParameters.second;
    if (upDownComparison.distance <= threshold && upDownComparison.isLegitimate == false)
      falseAcceptances++;
    if (upDownComparison.distance > threshold && upDownComparison.isLegitimate == true)
      falseRejections++;

    file << upDownComparison.distance << "," << upDownComparison.featureMeanDeviation << "," << upDownComparison.isLegitimate << "\n";
  }
  file.close();

  file.open("downUpSeparation.csv", std::ios::out | std::ios::app);
  file << "Distance,FeatureMeanDeviation,Legitimate\n";
  for (Comparison downUpComparison : downUpComparisons) {
    double threshold = bestDownUpParameters.first * downUpComparison.featureMeanDeviation + bestDownUpParameters.second;
    if (downUpComparison.distance <= threshold && downUpComparison.isLegitimate == false)
      falseAcceptances++;
    if (downUpComparison.distance > threshold && downUpComparison.isLegitimate == true)
      falseRejections++;

    file << downUpComparison.distance << "," << downUpComparison.featureMeanDeviation << "," << downUpComparison.isLegitimate << "\n";
  }
  file.close();

  double overall_FAR = (double)falseAcceptances / (double)impostorAttempts * 100;
  double overall_FRR = (double)falseRejections / (double)legitimateUserAttempts * 100;

  std::cout << "Overall False Acceptance Rate: " << overall_FAR << "%\n";
  std::cout << "Overall False Rejection Rate: " << overall_FRR << "%\n";

  return EXIT_SUCCESS;
}
