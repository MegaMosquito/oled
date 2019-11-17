//  get_http.c  Utility functions for interacting with remote HTTP servers
//  Written by mosquito@darlingevil.com, 2019-11-15


#include <errno.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 


// To use this you will normally want all of these set to zero
#define DEBUG   0
#define SHOWALL 0
#define ERRORS  0
#define TEST    0


// Arbitrary buffer sizes
#define SIZE_REQUEST  1024
#define SIZE_RESPONSE 4096


// Interface function to perform an HTTP GET
char* get_http(char* host, int port, char* url) {

  int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (tcpSocket < 0) {
    if (ERRORS) fprintf(stderr, "ERROR: Unable to create socket.\n");
    return NULL;
  } else {
    if (DEBUG) printf("Socket created.\n");
  }

  struct hostent *server;
  if ((server = gethostbyname(host)) == NULL) {
    if (ERRORS) fprintf(stderr, "ERROR: Cannot resolve hostname '%s'.\n", host);
    return NULL;
  } else {
    if (DEBUG) printf("Hostname resolved to: %s.\n", server->h_name);
  }

  struct sockaddr_in serverAddr;
  memcpy(&serverAddr.sin_addr, server->h_addr_list[0], server->h_length);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  if (connect(tcpSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
    if (ERRORS) fprintf(stderr, "ERROR: Unable to connect to %s:%d.\n", server->h_name, port);
    return NULL;
  } else {
    if (DEBUG) printf("Connected to %s:%d.\n", server->h_name, port);
  }

  char request[SIZE_REQUEST];
  snprintf(request, SIZE_REQUEST, "GET %s HTTP/1.1\r\n\r\n", url);
  if (send(tcpSocket, request, strlen(request), 0) < 0) {
    if (ERRORS) fprintf(stderr, "ERROR: Failed to send request: '%s'\n", request);
    return NULL;
  } else {
    if (DEBUG) printf("Request sent: %s\n", request);
  }

  static char response[SIZE_RESPONSE];
  int byte_count = read(tcpSocket, response, 4096 - 1);
  response[byte_count] = 0;
  if (DEBUG) {
    printf("Received: %d bytes.\n", byte_count);
    if (SHOWALL) printf("%s\n", response);
  }

  return response;
}

// Interface function to confirm connectivity to an HTTP server
int reachable(char* host, int port, char* url) {
  char* response = get_http(host, port, url);
  return (NULL != response);
}


// Test shell
#if TEST
int main() {

  void check(char* response) {
    if (NULL == response) {
      printf("Response: NULL\n");
    } else {
      printf("Response: %d bytes.\n", strlen(response));
      if (SHOWALL) printf("%s\n", response);
    }
  }

  void check2(int r) {
    if (r) {
      printf("Reachable: TRUE\n");
    } else {
      printf("Reachable: FALSE\n");
    }
  }

  void test(char* host, int port, char* url) {
    printf("\n\nTrying to GET %s:%d%s ...\n", host, port, url);
    char* response = get_http(host, port, url);
    if (!DEBUG) check(response);
  }

  void test2(char* host, int port, char* url) {
    printf("\n\nTrying to REACH %s:%d%s ...\n", host, port, url);
    int r = reachable(host, port, url);
    if (!DEBUG) check2(r);
  }

  test("10.10.10.10", 80, "/");
  test("google.com", 80, "/");
  test("wabonza.com", 80, "/");
  test("google.com", 80, "/not-a-valid-goog-link-methinks");
  test("192.168.123.249", 80, "/");

  test2("10.10.10.10", 80, "/");
  test2("google.com", 80, "/");
  test2("wabonza.com", 80, "/");
  test2("google.com", 80, "/not-a-valid-goog-link-methinks");
  test2("192.168.123.249", 80, "/");

  exit(0);
}
#endif // MAIN
