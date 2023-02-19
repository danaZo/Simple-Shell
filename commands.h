#pragma once

#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"
#define STDOUT 0
#define TCPOUT 1

void shLoop();

char * getCommand();

void executeCommand(char *);

void connectToServer();