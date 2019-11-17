//  run.c  Utility functions for running shell programs and getting output.
//  Written by mosquito@darlingevil.com, 2019-11-15


#include <errno.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 


// To use this you will normally want all of these set to zero
#define DEBUG   0
#define SHOWALL 0
#define ERRORS  0
#define TEST    0


// Caller must provide an output buffer
char* run(char* cmd, char* output, int size) {
  
  FILE *fp;
  fp = popen(cmd, "r");
  if (fp != NULL) {
    if (DEBUG) printf("Opened pipe to '%s'.\n", cmd);
    int length = 0;
    while (NULL != fgets(output + length, size - length - 1, fp)) {
      int n = strlen(output + length);
      if (SHOWALL) {
        printf("Received %d bytes.\n", n);
        printf("%s\n", output + length);
      }
      length += n;
    }
    if (length > 0) {
      output[length] = '\0';
    } else {
      if (ERRORS) fprintf(stderr, "ERROR: Empty response from '%s'.\n", cmd);
      return NULL;
    }
    pclose(fp);
  } else {
    if (ERRORS) fprintf(stderr, "ERROR: Unable to open '%s'.\n", cmd);
    return NULL;
  }
  if (DEBUG) printf("Result: %d bytes.\n", strlen(output));
  if (SHOWALL) printf("%s\n", output);
  return output;
}


// Test shell
#if TEST
#define SIZE_BUFFER 1024
int main() {

  char buffer[SIZE_BUFFER];

  void check(char* results) {
    if (NULL == results) {
      printf("Results: NULL\n");
    } else {
      printf("Results: %d bytes.\n", strlen(results));
      if (SHOWALL) printf("%s\n",results);
    }
  }

  void test(char* cmd) {
    printf("\n\nRunning command '%s' ...\n", cmd);
    char* results = run(cmd, buffer, SIZE_BUFFER);
    if (!DEBUG) check(results);
  }

  test("echo \"Hello, world.\"");
  test("ls *.c");
  test("zooba");
  test("zooba node list | jq .configstate.state");
  test("date -u");
#define UTC_TIME_COMMAND "sh -c \"date -u | awk '{printf \\\"UTC: %s\\\", \\$4}'\""
  test(UTC_TIME_COMMAND);
#define UPTIME_COMMAND "sh -c \"uptime | awk '{printf \\\"up: %s avg %.2f\\\", \\$3, \\$(NF-2)}'\""
  test(UPTIME_COMMAND);


  exit(0);
}
#endif // MAIN
