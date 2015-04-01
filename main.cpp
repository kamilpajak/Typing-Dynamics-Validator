#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>

void printEvent(input_event event) {
  std::string keyState;
  if (event.value == 0)
    keyState = "RELEASED";
  if (event.value == 1)
    keyState = "PRESSED";
  printf("%-8s | %3d | %ld.%06ld\n", keyState.c_str(), event.code,
         (long)event.time.tv_sec, (long)event.time.tv_usec);
}

struct keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

int main(void) {
  const char *devicePath = "/dev/input/by-id/"
                           "usb-Microsft_Microsoft_Wireless_Desktop_Receiver_3."
                           "1-event-kbd";
  // Otwórz plik
  int fileDescriptor = open(devicePath, O_RDONLY);

  // Sprawdź czy plik został otwarty poprawnie
  if (fileDescriptor == -1) {
    fprintf(stderr, "Cannot open %s: %s\n", devicePath, strerror(errno));
    return EXIT_FAILURE;
  }

  std::vector<input_event> events;
  std::vector<keystroke> keystrokes;

  int counter = 0;

  while (counter < 90) {
    struct input_event event;
    ssize_t numberOfBytesRead = read(fileDescriptor, &event, sizeof event);

    if (numberOfBytesRead == (ssize_t)-1) {
      if (errno == EINTR)
        continue;
      else
        break;
    }

    if (numberOfBytesRead != sizeof event) {
      errno = EIO;
      break;
    }

    if (event.type == EV_KEY && (event.value == 0 || event.value == 1)) {
      events.push_back(event);
      printEvent(event);
      counter++;
    }
  }

  for (int i = 0; i < events.size() - 1; i++)
    if (events[i].value == 1)
      for (int j = i + 1; j < events.size(); j++)
        if (events[i].code == events[j].code) {

          char bufferKeyDownTime[128];
          sprintf(bufferKeyDownTime, "%ld.%06ld", (long)events[i].time.tv_sec,
                  (long)events[i].time.tv_usec);
          double keyDownTime = atof(bufferKeyDownTime);

          char bufferKeyUpTime[128];
          sprintf(bufferKeyUpTime, "%ld.%06ld", (long)events[j].time.tv_sec,
                  (long)events[j].time.tv_usec);
          double keyUpTime = atof(bufferKeyUpTime);

          keystroke caughtKeystroke;
          caughtKeystroke.keyCode = events[i].code;
          caughtKeystroke.keyDownTime = keyDownTime;
          caughtKeystroke.keyUpTime = keyUpTime;
          keystrokes.push_back(caughtKeystroke);
          break;
        }

  for (int i = 0; i < keystrokes.size(); i++) {
    printf("Key Code:  %i\n", keystrokes[i].keyCode);
    printf("Key Down:  %f\n", keystrokes[i].keyDownTime);
    printf("Key Up:    %f\n\n", keystrokes[i].keyUpTime);
  }

  fflush(stdout);
  fprintf(stderr, "%s\n", strerror(errno));
  return EXIT_FAILURE;
}
