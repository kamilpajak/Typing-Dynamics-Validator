#include "config.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

// Summarized samples
struct Keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

std::vector<double> takeDownDownTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> downDownTimes;
  for (std::size_t i = 0; i < keystrokes.size() - 1; i++) {
    double downDownTime = keystrokes[i + 1].keyDownTime - keystrokes[i].keyDownTime;
    downDownTimes.push_back(downDownTime);
  }

  return downDownTimes;
}

std::vector<double> takeUpDownTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> upDownTimes;
  for (std::size_t i = 0; i < keystrokes.size() - 1; i++) {
    double upDownTime = keystrokes[i + 1].keyDownTime - keystrokes[i].keyUpTime;
    upDownTimes.push_back(upDownTime);
  }

  return upDownTimes;
}

std::vector<double> takeDownUpTimes(std::vector<Keystroke> keystrokes) {
  std::vector<double> downUpTimes;
  for (Keystroke keystroke : keystrokes) {
    double downUpTime = keystroke.keyUpTime - keystroke.keyDownTime;
    downUpTimes.push_back(downUpTime);
  }

  return downUpTimes;
}

struct Sample {
  std::vector<double> downDownTimes;
  std::vector<double> upDownTimes;
  std::vector<double> downUpTimes;
};

typedef std::map<int, std::map<int, Sample>> Samples;

Samples summarizeKeystrokeData(int minimumSamples) {
  Samples samples;

  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement *preparedStatement;
  sql::Connection *connection;

  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  preparedStatement = connection->prepareStatement("SELECT user_id FROM sample GROUP BY user_id HAVING count(*) >= ?;");
  preparedStatement->setInt(1, minimumSamples);
  sql::ResultSet *userIDs = preparedStatement->executeQuery();

  while (userIDs->next()) {
    preparedStatement = connection->prepareStatement("SELECT id FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, userIDs->getInt("user_id"));
    sql::ResultSet *sampleIDs = preparedStatement->executeQuery();

    while (sampleIDs->next()) {
      preparedStatement = connection->prepareStatement("SELECT keyCode, keyDownTime, keyUpTime FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, sampleIDs->getInt("id"));
      sql::ResultSet *keystrokeIDs = preparedStatement->executeQuery();

      std::vector<Keystroke> keystrokes;
      while (keystrokeIDs->next()) {
        Keystroke keystroke;
        keystroke.keyCode = keystrokeIDs->getInt("keyCode");
        keystroke.keyDownTime = keystrokeIDs->getDouble("keyDownTime");
        keystroke.keyUpTime = keystrokeIDs->getDouble("keyUpTime");
        keystrokes.push_back(keystroke);
      }

      delete keystrokeIDs;
      int userID = userIDs->getInt("user_id");
      int sampleID = sampleIDs->getInt("id");
      Sample sample;
      sample.downDownTimes = takeDownDownTimes(keystrokes);
      sample.upDownTimes = takeUpDownTimes(keystrokes);
      sample.downUpTimes = takeDownUpTimes(keystrokes);
      samples[userID][sampleID] = sample;
    }

    delete sampleIDs;
  }

  delete userIDs;
  delete preparedStatement;
  delete connection;

  return samples;
}

// Profile
struct Profile {
  std::vector<double> downDownMeans;
  std::vector<double> upDownMeans;
  std::vector<double> downUpMeans;
  std::vector<double> downDownStandardDeviations;
  std::vector<double> upDownStandardDeviations;
  std::vector<double> downUpStandardDeviations;
};

Profile takeProfile(std::vector<Sample> trainingSet) {
  Profile profile;
  double counter;

  // Means
  for (std::size_t i = 0; i < trainingSet[0].downDownTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j].downDownTimes[i];
    profile.downDownMeans.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0].upDownTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j].upDownTimes[i];
    profile.upDownMeans.push_back(counter / trainingSet.size());
  }

  for (std::size_t i = 0; i < trainingSet[0].downUpTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += trainingSet[j].downUpTimes[i];
    profile.downUpMeans.push_back(counter / trainingSet.size());
  }

  // Standard deviations
  for (std::size_t i = 0; i < trainingSet[0].downDownTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j].downDownTimes[i] - profile.downDownMeans[i]);
    profile.downDownStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0].upDownTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j].upDownTimes[i] - profile.upDownMeans[i]);
    profile.upDownStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  for (std::size_t i = 0; i < trainingSet[0].downUpTimes.size(); i++) {
    counter = 0;
    for (std::size_t j = 0; j < trainingSet.size(); j++)
      counter += std::abs(trainingSet[j].downUpTimes[i] - profile.downUpMeans[i]);
    profile.downUpStandardDeviations.push_back(counter / (trainingSet.size() - 1));
  }

  return profile;
}

// Classifier
struct Distance {
  double downDown;
  double upDown;
  double downUp;
};

Distance calculateDistance(Profile profile, Sample sample) {
  Distance distance;
  distance.downDown = 0;
  distance.upDown = 0;
  distance.downUp = 0;

  for (std::size_t i = 0; i < sample.downDownTimes.size(); i++)
    distance.downDown += (sample.downDownTimes[i] - profile.downDownMeans[i]) / profile.downDownStandardDeviations[i];
  distance.downDown /= sample.downDownTimes.size();

  for (std::size_t i = 0; i < sample.upDownTimes.size(); i++)
    distance.upDown += (sample.upDownTimes[i] - profile.upDownMeans[i]) / profile.upDownStandardDeviations[i];
  distance.upDown /= sample.upDownTimes.size();

  for (std::size_t i = 0; i < sample.downUpTimes.size(); i++)
    distance.downUp += (sample.downUpTimes[i] - profile.downUpMeans[i]) / profile.downUpStandardDeviations[i];
  distance.downUp /= sample.downUpTimes.size();

  return distance;
}

struct Threshold {
  double downDown;
  double upDown;
  double downUp;
};

Threshold determineThreshold(Profile profile) {
  Threshold threshold;
  threshold.downDown = 0;
  threshold.upDown = 0;
  threshold.downUp = 0;
  double meanOfStandardDeviations;

  meanOfStandardDeviations = 0;
  for (double downDownStandardDeviation : profile.downDownStandardDeviations)
    meanOfStandardDeviations += downDownStandardDeviation;
  meanOfStandardDeviations /= profile.downDownStandardDeviations.size();

  meanOfStandardDeviations = 0;
  for (double upDownStandardDeviation : profile.upDownStandardDeviations)
    meanOfStandardDeviations += upDownStandardDeviation;
  meanOfStandardDeviations /= profile.upDownStandardDeviations.size();

  meanOfStandardDeviations = 0;
  for (double downUpStandardDeviation : profile.downUpStandardDeviations)
    meanOfStandardDeviations += downUpStandardDeviation;
  meanOfStandardDeviations /= profile.downUpStandardDeviations.size();

  return threshold;
}

std::vector<std::vector<Sample>> getSubsetsOfSamples(std::size_t samplesPerProfile, const std::map<int, Sample> &samples) {
  if (samples.size() < samplesPerProfile)
    return {};

  std::vector<std::vector<Sample>> subsets;
  auto first = samples.begin();
  auto last = std::next(first, samplesPerProfile - 1);

  for (; last != samples.end(); ++first, ++last) {
    std::vector<Sample> subset;
    for (auto it = first; it != std::next(last); ++it)
      subset.push_back(it->second);
    subsets.push_back(subset);
  }

  return subsets;
}

// *** MAIN FUNCTION *** //

int main() {
  typedef std::map<int, std::vector<std::pair<std::vector<int>, Profile>>> Profiles;

  int samplesPerProfile = 10;
  Samples samples = summarizeKeystrokeData(samplesPerProfile);
  Profiles profiles;

  for (const auto &userID_map : samples) {
    std::vector<std::vector<Sample>> subsetsOfSamples = getSubsetsOfSamples(samplesPerProfile, userID_map.second);
  }

  return 0;
}
