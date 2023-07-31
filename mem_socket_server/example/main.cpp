#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <map>
#include <cstring>

#include "utils.h"
#include "db_impl.h"
#include "socket.h"
#include "eventloop.h"
#include "server.h"
#include "sqlmanager.h"
#include "config.h"
// server
#include "option.h"

int main () {
    EventLoop *loop = new EventLoop();
    Server *serv = new Server(loop);
    loop->loop();
}