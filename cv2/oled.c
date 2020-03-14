//  oled.c Display useful information on the 0.96" OLED display.
//  Uses this library: https://github.com/bitbank2/oled_96
//  Written by mosquito@darlingevil.com, 2019-11-15


#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "oled96.h"
#include "get_http.h"
#include "run.h"


#define DEBUG 0


#define STATE_COMMAND    "sh -c \"/usr2/horizon/bin/hzn node list | jq .configstate.state\" 2>/dev/null"
#define PATTERN_COMMAND  "sh -c \"/usr2/horizon/bin/hzn node list | jq .pattern\" 2>/dev/null"
#define POLICY_COMMAND   "sh -c \"/usr2/horizon/bin/hzn policy list | wc -l\" 2>/dev/null"
#define INTERNET_TARGET  "google.com"
#define UTC_TIME_COMMAND "sh -c \"date -u | awk '{printf \\\"UTC: %s\\\", \\$4}'\" 2>/dev/null"
#define UPTIME_COMMAND   "sh -c \"uptime | sed 's/.*  *up  */up /;s/,.*average:  */zzz/;s/,.*//;s/zzz/, avg /' | tr -d '\n'\" 2>/dev/null"


static volatile int loop = 1;
void intHandler(int sig) {
    loop = 0;
}

#define SLEEP_SEC 2
#define DISPLAY_CHARS 21
#define SIZE_LINE 128
#define SIZE_OUTPUT 2048
#define SPACES "                      "
int main(int argc, char* argv[]) {

  void centerString(int y, char* message) {
    if (DEBUG) printf("LINE: %d, MESSAGE: '%s'\n", y, message);
    char tmp[SIZE_OUTPUT];
    char* padding = "";
    int count = (DISPLAY_CHARS - strlen(message)) / 2;
    if (count > 0 && count < (DISPLAY_CHARS - 1))
      padding = &(SPACES[DISPLAY_CHARS - count]);
    (void) snprintf(tmp, SIZE_OUTPUT, "%s%s%s", padding, message, SPACES);
    oledWriteString(0, y, tmp, FONT_SMALL);
  }

  signal(SIGINT, intHandler);

  char* router = getenv("LOCAL_ROUTER_ADDRESS");
  char* ipv4 = getenv("LOCAL_IP_ADDRESS");
  if ((NULL == router) || (NULL == ipv4)) {
    fprintf(stderr, "ERROR: Required environment variables are not set.\n");
    exit(-1);
  }

  int iOLEDAddr = 0x3c;
  int iOLEDType = OLED_128x64;
  int bFlip = 0, bInvert = 0;
  int i = -1;
  int iChannel = -1;
  while (i != 0 && iChannel < 2) {
    iChannel++;
    i=oledInit(iChannel, iOLEDAddr, iOLEDType, bFlip, bInvert);
  }
  if (0 != i) {
    fprintf(stderr, "Unable to initialize I2C bus 0-2, please check your connections: 'i2cdetect -l' and 'i2cdetect -y <channel>'\n");
    exit(-1);
  }
  oledFill(0);

  char line[SIZE_LINE];
  char state[SIZE_OUTPUT];
  char pattern[SIZE_OUTPUT];
  char policy[SIZE_OUTPUT];
  char utc_time[SIZE_OUTPUT];
  char uptime[SIZE_OUTPUT];
  while(loop) {

    char* hzn = run(STATE_COMMAND, state, SIZE_OUTPUT);
    int gateway_reachable = reachable(router, 80, "/");
    int internet_reachable = reachable(INTERNET_TARGET, 80, "/");
    (void) run(UTC_TIME_COMMAND, utc_time, SIZE_OUTPUT);
    if (DEBUG) printf("UTC_TIME: '%s'\n", utc_time);
    (void) run(UPTIME_COMMAND, uptime, SIZE_OUTPUT);
    if (DEBUG) printf("UPTIME: '%s'\n", uptime);

    int l = 0;
    (void) snprintf(line, SIZE_LINE - 1, "IPv4: %s", ipv4);
    centerString(l++, line);

    if (NULL == hzn) {
      (void) snprintf(line, SIZE_LINE - 1, "(hzn not installed)");
    } else {
      state[strlen(state) - 1] = '\0';
      (void) snprintf(line, SIZE_LINE - 1, state);
    }
    centerString(l++, line);

    if ((NULL != hzn) && (0 == strcmp(hzn, "\"configured\""))) {
      char* p = run(PATTERN_COMMAND, pattern, SIZE_OUTPUT);
      if (NULL != p) {
        pattern[strlen(pattern) - 1] = '\0';
        if (0 == strcmp(pattern, "null")) {
          // Error? Cannot happen!
        } else if (0 == strcmp(pattern, "\"\"")) {
          p = run(POLICY_COMMAND, policy, SIZE_OUTPUT);
          policy[strlen(policy) - 1] = '\0';
          if (0 == strcmp(policy, "2")) {
            // Error? Cannot happen!
          } else {
            (void) snprintf(line, SIZE_LINE - 1, "(using policy)");
          }
          centerString(l++, line);
        } else {
          pattern[strlen(pattern) - 1] = '\0';
          p = pattern + 1;
          (void) snprintf(line, SIZE_LINE - 1, "%s", p);
          centerString(l++, line);
        }
      }
    }

    if (gateway_reachable) {
      (void) snprintf(line, SIZE_LINE - 1, " Gateway: (connected)");
    } else {
      (void) snprintf(line, SIZE_LINE - 1, " Gateway: UNREACHABLE!");
    }
    centerString(l++, line);

    if (internet_reachable) {
      (void) snprintf(line, SIZE_LINE - 1, "Internet: (reachable)");
    } else {
      (void) snprintf(line, SIZE_LINE - 1, "Internet: UNREACHABLE!");
    }
    centerString(l++, line);

#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat-truncation"

    (void) snprintf(line, SIZE_LINE - 1, "%s", utc_time);
    centerString(l++, line);

    (void) snprintf(line, SIZE_LINE - 1, "%s", uptime);
    centerString(l++, line);

#   pragma GCC diagnostic pop

    (void) snprintf(line, SIZE_LINE - 1, SPACES);
    for (;l < 8; i++) {
      centerString(l, line);
    }

    sleep(SLEEP_SEC);
  }

  exit(0);
}
