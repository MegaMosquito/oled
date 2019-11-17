//  get_http.h  Utility functions for interacting with remote HTTP servers
//  Written by mosquito@darlingevil.com, 2019-11-15

extern char* get_http(char* host, int port, char* url);
extern int reachable(char* host, int port, char* url);

