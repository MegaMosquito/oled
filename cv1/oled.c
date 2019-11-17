//  oled.c Display useful information on the 0.96" OLED display.
//  https://github.com/stevelorenz/codes-on-rasp/tree/master/c-rasp/ssd1306-i2c
//  Written by mosquito@darlingevil.com, 2019-11-15


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-sign"

#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#pragma GCC diagnostic pop

#include "ssd1306_i2c.h"
#include "get_http.h"
#include "run.h"


#define INTERNET_TARGET "google.com"
#define UTC_TIME_COMMAND "sh -c \"date -u | awk '{printf \\\"UTC: %s\\\", \\$4}'\""
#define UPTIME_COMMAND   "sh -c \"uptime | awk '{printf \\\"up: %s avg %.2f\\\", \\$3, \\$(NF-2)}'\""



static volatile int loop = 1;
void intHandler(int sig) {
    loop = 0;
}

#define SLEEP_SEC 2
#define SIZE_LINE 128
#define SIZE_OUTPUT 2048
int main(int argc, char* argv[]) {

  signal(SIGINT, intHandler);

  char* router = getenv("LOCAL_ROUTER_ADDRESS");
  char* ipv4 = getenv("LOCAL_IP_ADDRESS");
  if ((NULL == router) || (NULL == ipv4)) {
    fprintf(stderr, "ERROR: Required environment variables are not set.\n");
    exit(-1);
  }

  int fd = ssd1306I2CSetup(0x3C);
  if (!fd) {
    fprintf(stderr, "ERROR: Unable to initialize the ssd1306 I2C interface.\n");
    exit(-1);
  }
  displayOn(fd);  

  unsigned char line[SIZE_LINE];
  char utc_time[SIZE_OUTPUT];
  char uptime[SIZE_OUTPUT];
  while(loop) {

    int gateway_reachable = reachable(router, 80, "/");
    int internet_reachable = reachable(INTERNET_TARGET, 80, "/");
    (void) run(UTC_TIME_COMMAND, utc_time, SIZE_OUTPUT);
    (void) run(UPTIME_COMMAND, uptime, SIZE_OUTPUT);

    // clearDisplay(fd);
    int l = 1;

    (void) snprintf((char*)line, SIZE_LINE - 1, "ipv4: %s", ipv4);
    draw_line(l++, 1, line);

    if (gateway_reachable) {
      (void) snprintf((char*)line, SIZE_LINE - 1, "Gwy: (reachable)");
    } else {
      (void) snprintf((char*)line, SIZE_LINE - 1, "Gwy: UNREACHABLE!");
    }
    draw_line(l++, 1, line);

    if (internet_reachable) {
      (void) snprintf((char*)line, SIZE_LINE - 1, "Int: (connected)");
    } else {
      (void) snprintf((char*)line, SIZE_LINE - 1, "Int: UNREACHABLE!");
    }
    draw_line(l++, 1, line);

    //(void) snprintf((char*)line, SIZE_LINE - 1, " ");
    //draw_line(l++, 1, line);

#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat-truncation"

    (void) snprintf((char*)line, SIZE_LINE - 1, "%s", utc_time);
    draw_line(l++, 1, line);

    //(void) snprintf((char*)line, SIZE_LINE - 1, "%s", uptime);
    //draw_line(l++, 1, line);

#   pragma GCC diagnostic pop

    updateDisplay(fd);
    sleep(SLEEP_SEC);
  }

  return 0;
}
