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

  std::vector<keystroke> keystrokes;
  int counter = 0;
  while (counter < 10) {
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

    if (event.type == EV_KEY) {
      char buffer[18];
      sprintf(buffer, "%ld.%06ld", (long)event.time.tv_sec,
              (long)event.time.tv_usec);
      double timestamp = atof(buffer);

      if (event.value == 1) { // Klawisz wciśnięto
        keystroke caughtKeystroke;
        caughtKeystroke.keyCode = event.code;
        caughtKeystroke.keyDownTime = timestamp;
        caughtKeystroke.keyUpTime = 0;

        keystrokes.push_back(caughtKeystroke);
      }

      if (event.value == 0) { // Klawisz puszczono
        for (int i = 0; i < keystrokes.size(); i++)
          if (keystrokes[i].keyCode == event.code &&
              keystrokes[i].keyUpTime == 0) {
            keystrokes[i].keyUpTime = timestamp;
            counter++;
            break;
          }
      }
    }
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

