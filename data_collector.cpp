#include "config.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

// Input device
const std::string COMMAND_GET_INPUT_DEVICE_NAME =
    "grep -E 'Name=|EV=' /proc/bus/input/devices |"
    "grep -B1 'EV=120013' |"
    "grep -Po '(?<=\")(.*?)(?=\")' |"
    "tr -d '\n'";

const std::string COMMAND_GET_INPUT_DEVICE_EVENT_NUMBER =
    "grep -E 'Handlers|EV=' /proc/bus/input/devices |"
    "grep -B1 'EV=120013' |"
    "grep -Eo 'event[0-9]+' |"
    "grep -Eo '[0-9]+' |"
    "tr -d '\n'";

std::string executeCommand(std::string command) {
  FILE *pipe = popen(command.c_str(), "r");
  char buffer[128];
  std::string result = "";
  while (!feof(pipe))
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  pclose(pipe);
  return result;
}

std::string getInputDeviceName() {
  return executeCommand(COMMAND_GET_INPUT_DEVICE_NAME.c_str());
}

std::string getInputDevicePath() {
  return "/dev/input/event" +
         executeCommand(COMMAND_GET_INPUT_DEVICE_EVENT_NUMBER.c_str());
}

// Raw events
enum { KEY_RELEASED, KEY_PRESSED, KEY_REPEATED };

bool isEventValid(input_event event) {
  if (event.type == EV_KEY)
    if (event.code != 28 && event.code != 96)
      if (event.value != KEY_REPEATED)
        return true;
  return false;
}

bool isEnterPressed(input_event event) {
  if (event.type == EV_KEY)
    if (event.code == 28 || event.code == 96)
      if (event.value == KEY_PRESSED)
        return true;
  return false;
}

std::vector<input_event> getEvents() {
  std::vector<input_event> events;
  std::string devicePath = getInputDevicePath();
  int fileDescriptor = open(devicePath.c_str(), O_RDONLY);
  struct input_event event;
  while (true) {
    read(fileDescriptor, &event, sizeof(struct input_event));
    if (isEventValid(event)) {
      events.push_back(event);
      continue;
    }
    if (isEnterPressed(event))
      break;
  }
  close(fileDescriptor);
  return events;
}

// Keystroke data
struct Keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

std::vector<Keystroke> takeKeystrokes(std::vector<input_event> events) {
  std::vector<Keystroke> keystrokes;
  for (unsigned int i = 0; i < events.size() - 1; i++)
    if (events[i].value == KEY_PRESSED)
      for (unsigned int j = i + 1; j < events.size(); j++)
        if (events[i].code == events[j].code) {
          double keyDownTime =
              events[i].time.tv_sec + (double)events[i].time.tv_usec / 1000000;
          double keyUpTime =
              events[j].time.tv_sec + (double)events[j].time.tv_usec / 1000000;
          Keystroke keystroke;
          keystroke.keyCode = events[i].code;
          keystroke.keyDownTime = keyDownTime;
          keystroke.keyUpTime = keyUpTime;
          keystrokes.push_back(keystroke);
          break;
        }
  return keystrokes;
}

// Text and key codes verification (HARD CODED)
bool isProvidedStringCorrect(std::string providedString) {
  return (providedString == "Uniwersytet Slaski");
}

bool areKeyCodesCorrect(std::vector<Keystroke> providedKeystrokes) {
  std::vector<int> permittedKeyCodes = {42, 22, 49, 23, 17, 18, 19, 31, 21, 20,
                                        18, 20, 57, 42, 31, 38, 30, 31, 37, 23};

  std::vector<int> providedKeyCodes;
  for (unsigned int i = 0; i < providedKeystrokes.size(); i++)
    providedKeyCodes.push_back(providedKeystrokes[i].keyCode);

  sort(providedKeyCodes.begin(), providedKeyCodes.end());
  sort(permittedKeyCodes.begin(), permittedKeyCodes.end());

  return (providedKeyCodes == permittedKeyCodes);
}

// Database
bool login(std::string username, std::string password) {

  sql::mysql::MySQL_Driver *driver;
  driver = sql::mysql::get_mysql_driver_instance();
  sql::Connection *connection;
  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  sql::Statement *statement;
  statement = connection->createStatement();
  sql::ResultSet *result;
  result = statement->executeQuery("SELECT * FROM user ORDER BY id ASC");
  bool isLogged = false;
  while (result->next()) {
    if (result->getString("username") == username)
      if (result->getString("password") == password) {
        isLogged = true;
        break;
      }
  }
  delete result;
  delete statement;
  delete connection;
  return isLogged;
}

void uploadData(std::string username, std::string inputDeviceName,
                std::vector<Keystroke> keystrokes) {
  sql::mysql::MySQL_Driver *driver;
  driver = sql::mysql::get_mysql_driver_instance();
  sql::Connection *connection;
  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");

  sql::Statement *statement;
  statement = connection->createStatement();
  sql::ResultSet *result;
  result = statement->executeQuery(std::string("SELECT id FROM user WHERE username = '" + username + "\'").c_str());
  int userID;
  while (result->next())
    userID = result->getInt("id");

  statement->execute("START TRANSACTION;");
  sql::PreparedStatement *preparedStatement;
  preparedStatement = connection->prepareStatement("INSERT INTO sample(input_device, user_id) VALUES (?, ?)");
  preparedStatement->setString(1, inputDeviceName);
  preparedStatement->setInt(2, userID);
  preparedStatement->executeUpdate();

  result = statement->executeQuery("SELECT LAST_INSERT_ID()");
  int sampleID;
  while (result->next())
    sampleID = result->getInt("LAST_INSERT_ID()");

  preparedStatement = connection->prepareStatement("INSERT INTO keystroke(keyCode, keyDownTime, keyUpTime, sample_id) VALUES (?, ?, ?, ?)");
  for (Keystroke keystroke : keystrokes) {
    preparedStatement->setInt(1, keystroke.keyCode);
    preparedStatement->setDouble(2, keystroke.keyDownTime);
    preparedStatement->setDouble(3, keystroke.keyUpTime);
    preparedStatement->setInt(4, sampleID);
    preparedStatement->executeUpdate();
  }
  statement->execute("COMMIT;");

  delete preparedStatement;
  delete result;
  delete statement;
  delete connection;
}

// *** MAIN FUNCTION *** //

int main() {
  std::string username;
  std::string password;

  std::cout << "Username: ";
  std::cin >> username;
  std::cin.ignore();
  std::cout << "Password: ";
  std::cin >> password;
  std::cin.ignore();

  if (login(username, password)) {
    std::string inputDeviceName = getInputDeviceName();
    std::cout << "You are logged in as " << username << std::endl;
    char selection;
    while (true) {
      std::cout << "Please type \"Uniwersytet Slaski\"" << std::endl;
      std::string providedString;
      auto future = std::async(std::launch::async, getEvents);
      std::getline(std::cin, providedString);
      std::vector<input_event> events = future.get();

      if (isProvidedStringCorrect(providedString)) {
        std::vector<Keystroke> keystrokes = takeKeystrokes(events);
        if (areKeyCodesCorrect(keystrokes)) {
          std::cout << "Thank you! ";
          uploadData(username, inputDeviceName, keystrokes);
        } else
          std::cout << "Keystrokes collection is not correct. ";
      } else
        std::cout << "Provided string is not correct. ";
      std::cout << "Do you want to type again? [Y/n] ";
      std::cin >> selection;
      std::cin.ignore();
      if (selection == 'n' || selection == 'N')
        break;
    }
  } else
    std::cout << "Username and password do not match" << std::endl;

  return 0;
}
