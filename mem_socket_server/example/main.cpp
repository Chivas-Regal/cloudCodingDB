#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <map>
#include <cstring>

#include "util.h"
#include "socket.h"
#include "eventloop.h"
#include "server.h"
#include "usrinfo/sqlmanager.h"
#include "config.h"
// server

int main () {
    EventLoop *loop = new EventLoop();
    Server *serv = new Server(loop);
    loop->loop();
}