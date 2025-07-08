#ifndef _SERVER_WEB_H_
#define _SERVER_WEB_H_

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<iostream>
#include<string.h>
#include "Server_Log.h"

extern bool User_Quit;

int Server_Web(int argc, char **argv);



#endif