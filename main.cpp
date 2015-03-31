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

int main(void) {
  const char *devicePath = "/dev/input/by-id/"
                           "usb-Microsft_Microsoft_Wireless_Desktop_Receiver_3."
                           "1-event-kbd";
  int fileDescriptor = open(devicePath, O_RDONLY);

  if (fileDescriptor == -1) {
    fprintf(stderr, "Cannot open %s: %s\n", devicePath, strerror(errno));
    return EXIT_FAILURE;
  }

  while (1) {
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

    if (event.type == EV_KEY && event.value >= 0 && event.value <= 1) {
      // events.push_back(event);
      printEvent(event);
    }
  }

  fflush(stdout);
  fprintf(stderr, "%s\n", strerror(errno));
  return EXIT_FAILURE;
}
