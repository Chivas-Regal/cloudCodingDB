# include <sys/socket.h>
# include <arpa/inet.h>
# include <string>
# include <unistd.h>
# include <iostream>
# include <map>
#include <cstring>

#include "../include/util.h"
#include "../include/socket.h"
#include "../include/eventloop.h"
#include "../include/server.h"
#include "../include/usrinfo/sqlmanager.h"
// server

int main () {
    EventLoop *loop = new EventLoop();
    Server *serv = new Server(loop);
    loop->loop();
}