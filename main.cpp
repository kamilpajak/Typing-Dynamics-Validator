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

// Keystroke Data
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

// Menu
void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color) {
  int length, x, y;
  float temp;

  if (win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if (startx != 0)
    x = startx;
  if (starty != 0)
    y = starty;
  if (width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length) / 2;
  x = startx + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}

void showMenu() {
  char *choices[] = {"Choice 1", "Choice 2", "Choice 3",
                     "Choice 4", "Exit",     (char *)NULL};

  ITEM **my_items;
  int c;
  MENU *my_menu;
  WINDOW *my_menu_win;
  int n_choices, i;

  /* Initialize curses */
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);
  curs_set(0);

  /* Create items */
  n_choices = sizeof(choices) / sizeof(choices[0]);
  my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
  for (i = 0; i < n_choices; ++i)
    my_items[i] = new_item(choices[i], choices[i]);

  /* Crate menu */
  my_menu = new_menu((ITEM **)my_items);

  /* Create the window to be associated with the menu */
  my_menu_win = newwin(10, 40, 4, 4);
  keypad(my_menu_win, TRUE);

  /* Set main window and sub window */
  set_menu_win(my_menu, my_menu_win);
  set_menu_sub(my_menu, derwin(my_menu_win, 6, 38, 3, 1));

  /* Set menu mark to the string " * " */
  set_menu_mark(my_menu, " * ");

  /* Print a border around the main window and print a title */
  box(my_menu_win, 0, 0);
  print_in_middle(my_menu_win, 1, 0, 40, "Typing Dynamics Validator", COLOR_PAIR(1));
  mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
  mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
  refresh();

  /* Post the menu */
  post_menu(my_menu);
  wrefresh(my_menu_win);

  while ((c = wgetch(my_menu_win)) != KEY_F(1)) {
    switch (c) {
    case KEY_DOWN:
      menu_driver(my_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(my_menu, REQ_UP_ITEM);
      break;
    }
    wrefresh(my_menu_win);
  }

  /* Unpost and free all the memory taken up */
  unpost_menu(my_menu);
  free_menu(my_menu);
  for (i = 0; i < n_choices; ++i)
    free_item(my_items[i]);
  endwin();
}

// *** MAIN FUNCTION *** //

int main() {
  std::string devicePath = "/dev/input/by-id/"
                           "usb-Microsft_Microsoft_Wireless_Desktop_Receiver_3."
                           "1-event-kbd";

  showMenu();

  return 0;
}
