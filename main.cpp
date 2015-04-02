#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>

// --- KEYSTROKE --- //

struct keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

// ----------------- //

// ---- FUNKCJE ---- //

std::vector<keystroke> takeKeystrokes(std::vector<input_event> events) {
  std::vector<keystroke> keystrokes;

  for (int i = 0; i < events.size() - 1; i++)
    if (events[i].value == 1)
      for (int j = i + 1; j < events.size(); j++)
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

std::vector<double> takeDownDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downDownLatencies;
  for (int i = 1; i < keystrokes.size(); i++) {
    double downDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyDownTime;
    downDownLatencies.push_back(downDownLatency);
  }

  return downDownLatencies;
}

std::vector<double> takeUpDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> upDownLatencies;
  for (int i = 1; i < keystrokes.size(); i++) {
    double upDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyUpTime;
    upDownLatencies.push_back(upDownLatency);
  }

  return upDownLatencies;
}

std::vector<double> takeDownUpLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downUpLatencies;
  for (int i = 0; i < keystrokes.size(); i++) {
    double downUpLatency = keystrokes[i].keyUpTime - keystrokes[i].keyDownTime;
    downUpLatencies.push_back(downUpLatency);
  }

  return downUpLatencies;
}

void printEvent(input_event event) {
  std::string keyState;
  if (event.value == 0)
    keyState = "RELEASED";
  if (event.value == 1)
    keyState = "PRESSED";
  printf("%-8s | %3d | %ld.%06ld\n", keyState.c_str(), event.code,
         (long)event.time.tv_sec, (long)event.time.tv_usec);
}

// *** GŁÓWNA FUNKCJA *** //

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

  while (counter < 11) {
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
      counter++;
    }
  }

  keystrokes = takeKeystrokes(events);

  for (int i = 0; i < keystrokes.size(); i++) {
    printf("Key Code:  %i\n", keystrokes[i].keyCode);
    printf("Key Down:  %f\n", keystrokes[i].keyDownTime);
    printf("Key Up:    %f\n\n", keystrokes[i].keyUpTime);
  }

  takeDownUpLatencies(keystrokes);

  fflush(stdout);
  fprintf(stderr, "%s\n", strerror(errno));
  return EXIT_FAILURE;
}
