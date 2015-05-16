#include <linux/input.h>
#include <unistd.h> // read()
#include <fcntl.h>  // open()
#include <errno.h>
#include <error.h>
#include <ncurses.h>
#include <menu.h>
#include <cstdlib>
#include <vector>
#include <string>

// --- CONSTANTS --- //

enum { KEY_RELEASED, KEY_PRESSED, KEY_REPEATED };

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

std::string getInputDeviceName() {}

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
  // @ Initialize curses
  initscr();
  start_color();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);

  // @ Setup menu
  std::vector<std::string> menuChoices;
  menuChoices.push_back(" < CREATE > ");
  menuChoices.push_back(" < VERIFY > ");
  menuChoices.push_back(" <  EXIT  > ");

  ITEM **menuItems = (ITEM **)calloc(menuChoices.size() + 1, sizeof(ITEM *));
  for (unsigned int i = 0; i < menuChoices.size(); i++)
    menuItems[i] = new_item(menuChoices[i].c_str(), menuChoices[i].c_str());
  menuItems[menuChoices.size()] = (ITEM *)NULL;
  MENU *menu = new_menu((ITEM **)menuItems);

  set_menu_mark(menu, NULL);
  menu_opts_off(menu, O_SHOWDESC);
  set_menu_format(menu, 1, menuChoices.size());

  // @ Setup window
  const int windowHeight = 6;
  const int windowWidth = 70;
  WINDOW *window = newwin(windowHeight, windowWidth, (LINES - windowHeight) / 2,
                          (COLS - windowWidth) / 2);
  keypad(window, TRUE);
  box(window, 0, 0);
  std::string title = " Typing Dynamics Validator ";
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, 0, (windowWidth - title.length()) / 2, title.c_str());
  wattroff(window, COLOR_PAIR(1));
  const int menuHeight = 1;
  const int menuWidth = 38;
  set_menu_win(menu, window);
  set_menu_sub(menu, derwin(window, menuHeight, menuWidth, windowHeight - 1,
                            (windowWidth - menuWidth) / 2));

  // @ Post the menu
  post_menu(menu);
  wrefresh(window);

  int keyPressed;
  while ((keyPressed = wgetch(window))) {
    switch (keyPressed) {
    case KEY_RIGHT:
      menu_driver(menu, REQ_RIGHT_ITEM);
      break;
    case KEY_LEFT:
      menu_driver(menu, REQ_LEFT_ITEM);
      break;
    }
    wrefresh(window);
  }

  // @ Unpost and free all the memory taken up
  unpost_menu(menu);
  for (unsigned int i = 0; i < menuChoices.size() + 1; i++)
    free_item(menuItems[i]);
  free_menu(menu);
  endwin();
}

// *** MAIN FUNCTION *** //

int main() {
  std::string devicePath = "/dev/input/by-id/"
                           "usb-Microsft_Microsoft_Wireless_Desktop_Receiver_3."
                           "1-event-kbd";
  showMainView();
  return 0;
}
