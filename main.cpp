#include "config.h"

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

// --- CONSTANTS --- //

enum { KEY_RELEASED, KEY_PRESSED, KEY_REPEATED };
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

// --- STRUCTURES --- //

struct keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

// --- KEYSTROKE FUNCTIONS --- //

// Input device
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

// Sample
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

void clearInputBuffer() {
  int character;
  char buffer[BUFSIZ];

  while ((character = std::cin.get()) != '\n' && character != EOF);
  std::cin.getline(buffer, sizeof(buffer));
}

std::vector<input_event> getSample() {
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
  clearInputBuffer();
  return events;
}

// Keystroke data
std::vector<keystroke> takeKeystrokes(std::vector<input_event> events) {
  std::vector<keystroke> keystrokes;
  for (unsigned int i = 0; i < events.size() - 1; i++)
    if (events[i].value == KEY_PRESSED)
      for (unsigned int j = i + 1; j < events.size(); j++)
        if (events[i].code == events[j].code) {
          double keyDownTime =
              events[i].time.tv_sec + (double)events[i].time.tv_usec / 1000000;
          double keyUpTime =
              events[j].time.tv_sec + (double)events[j].time.tv_usec / 1000000;
          keystroke caughtKeystroke;
          caughtKeystroke.keyCode = events[i].code;
          caughtKeystroke.keyDownTime = keyDownTime;
          caughtKeystroke.keyUpTime = keyUpTime;
          keystrokes.push_back(caughtKeystroke);
          break;
        }
  return keystrokes;
}

// Features
std::vector<int> takeKeyCodes(std::vector<keystroke> keystrokes) {
  std::vector<int> keyCodes;
  for (unsigned int i = 0; i < keystrokes.size(); i++)
    keyCodes.push_back(keystrokes[i].keyCode);
  return keyCodes;
}

std::vector<double> takeDownDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downDownLatencies;
  for (unsigned int i = 1; i < keystrokes.size(); i++) {
    double downDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyDownTime;
    downDownLatencies.push_back(downDownLatency);
  }
  return downDownLatencies;
}

std::vector<double> takeUpDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> upDownLatencies;
  for (unsigned int i = 1; i < keystrokes.size(); i++) {
    double upDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyUpTime;
    upDownLatencies.push_back(upDownLatency);
  }
  return upDownLatencies;
}

std::vector<double> takeDownUpLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downUpLatencies;
  for (unsigned int i = 0; i < keystrokes.size(); i++) {
    double downUpLatency = keystrokes[i].keyUpTime - keystrokes[i].keyDownTime;
    downUpLatencies.push_back(downUpLatency);
  }
  return downUpLatencies;
}

// Template

// Classifier

// *** MAIN FUNCTION *** //

int main() {
  std::string username;
  std::string password;

  std::cout << "Username: ";
  std::cin >> username;
  std::cout << "Password: ";
  std::cin >> password;

  sql::mysql::MySQL_Driver *driver;
  sql::Connection *connection;
  sql::Statement *statement;
  sql::ResultSet *result;

  driver = sql::mysql::get_mysql_driver_instance();
  connection = driver->connect(HOST, USER, PASSWORD);

  bool isLogged = false;
  statement = connection->createStatement();
  statement->execute("USE typing_dynamics_validator");
  result = statement->executeQuery("SELECT * FROM user ORDER BY id ASC");
  while (result->next()) {
    if (result->getString("username") == username)
      if (result->getString("password") == password) {
        isLogged = true;
        break;
      }
  }

  if (isLogged) {
    std::cout << "You are logged in as " << username << std::endl;
    std::cout << "Please type \"Uniwersytet Slaski\"" << std::endl;
    std::vector<input_event> sample = getSample();
  }
  else
    std::cout << "Username and password do not match" << std::endl;

  delete result;
  delete statement;
  delete connection;

  return 0;
}
