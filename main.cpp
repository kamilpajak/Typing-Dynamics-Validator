#include <linux/input.h>
#include <fcntl.h>
#include <error.h>
#include <cdk/cdk.h>
#include <mysql_driver.h>
#include <vector>
#include <string>

// --- CONSTANTS --- //

enum { KEY_RELEASED, KEY_PRESSED, KEY_REPEATED };
static const std::string COMMAND_GET_INPUT_DEVICE_NAME =
    "grep -E 'Name=|EV=' /proc/bus/input/devices |"
    "grep -B1 'EV=120013' |"
    "grep -Po '(?<=\")(.*?)(?=\")' |"
    "tr -d '\n'";
static const std::string COMMAND_GET_INPUT_DEVICE_EVENT_NUMBER =
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
std::string executeCommand(const char *cmd) {
  FILE *pipe = popen(cmd, "r");
  if (!pipe)
    error(EXIT_FAILURE, errno, "Pipe error");
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
  while (true) {
    character = getch();
    if (character == '\n' || character == EOF)
      break;
  }
}

std::vector<input_event> getSample(std::string devicePath) {
  std::vector<input_event> events;
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

// --- USER INTERFACE --- //

// Main view
void showMainView() {
  CDKSCREEN *cdkscreen;
  CDKLABEL *demo;
  WINDOW *screen;
  char *mesg[4];

  /* Initialize the Cdk screen.   */
  screen = initscr();
  cdkscreen = initCDKScreen(screen);

  /* Start CDK Colors */
  initCDKColor();

  /* Set the labels up.      */
  mesg[0] = "</31>This line should have a yellow foreground and a cyan "
            "background.<!31>";
  mesg[1] = "</05>This line should have a white  foreground and a blue "
            "background.<!05>";
  mesg[2] = "</26>This line should have a yellow foreground and a red  "
            "background.<!26>";
  mesg[3] = "<C>This line should be set to whatever the screen default is.";

  /* Declare the labels.     */
  demo = newCDKLabel(cdkscreen, CENTER, CENTER, mesg, 4, TRUE, TRUE);

  /* Draw the label          */
  drawCDKLabel(demo, TRUE);
  waitCDKLabel(demo, ' ');

  /* Clean up           */
  destroyCDKLabel(demo);
  destroyCDKScreen(cdkscreen);
  endCDK();
}

// *** MAIN FUNCTION *** //

int main() {
  showMainView();
  return 0;
}
