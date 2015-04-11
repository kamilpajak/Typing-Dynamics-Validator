#include <linux/input.h>
#include <unistd.h> // read()
#include <fcntl.h>  // open()
#include <ncurses.h>
#include <menu.h>
#include <cstdlib>
#include <cstring>
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
    if (events[i].value == 1)
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
  char *choices[] = {" < CREATE > ", " < VERIFY > ", " <  EXIT  > ",
                     (char *)NULL};

  /* Initialize curses */
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);
  curs_set(0);

  /* Create the window */
  int terminal_height, terminal_width;
  getmaxyx(stdscr, terminal_height, terminal_width);
  const int window_height = 10;
  const int window_width = 70;

  WINDOW *my_win =
      newwin(window_height, window_width, (terminal_height - window_height) / 2,
             (terminal_width - window_width) / 2);
  keypad(my_win, TRUE);

  /* Create menu items */
  int n_choices = sizeof(choices) / sizeof(*choices);
  ITEM **my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
  for (int i = 0; i < n_choices; i++)
    my_items[i] = new_item(choices[i], choices[i]);

  /* Crate menu */
  MENU *my_menu = new_menu((ITEM **)my_items);

  /* Set menu options */
  menu_opts_off(my_menu, O_SHOWDESC);
  set_menu_format(my_menu, 1, 3);

  /* Set main window and sub window */
  set_menu_win(my_menu, my_win);
  int menu_height = 1;
  int menu_width = 38;
  set_menu_sub(my_menu,
               derwin(my_win, menu_height, menu_width, window_height - 1,
                      (window_width - menu_width) / 2));

  /* Set menu mark to the string */
  set_menu_mark(my_menu, NULL);

  /* Print a border around the main window and print a title */
  box(my_win, 0, 0);
  std::string title = " Typing Dynamics Validator ";
  int title_length = strlen(title.c_str());
  wattron(my_win, COLOR_PAIR(1));
  mvwprintw(my_win, 0, (window_width - title_length) / 2, "%s", title.c_str());
  wattroff(my_win, COLOR_PAIR(1));
  refresh();

  /* Post the menu */
  post_menu(my_menu);
  wrefresh(my_win);
  int c;
  while ((c = wgetch(my_win))) {
    switch (c) {
    case KEY_RIGHT:
      menu_driver(my_menu, REQ_RIGHT_ITEM);
      break;
    case KEY_LEFT:
      menu_driver(my_menu, REQ_LEFT_ITEM);
      break;
    case 10: /* Enter */
      break;
    }
    wrefresh(my_win);
  }

  /* Unpost and free all the memory taken up */
  unpost_menu(my_menu);
  free_menu(my_menu);
  for (int i = 0; i < n_choices; i++)
    free_item(my_items[i]);
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
