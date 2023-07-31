#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <map>
#include <cstring>
#include <thread>
#include <vector>
#include <sys/wait.h>

#include "util.h"
#include "socket.h"
#include "connector.h"
// client

struct node {
    int x, y;
    node (int _x = 0, int _y = 0) :
        x(_x),
        y(_y) {}
};


void sonWork (int id) {
    Connector *conn = new Connector();
    conn->Login("test" + std::to_string(id), "test" + std::to_string(id));

    /* Write array<node> : {{0,1}, {1,2}, {2,3}, ..., {9,10}} */
    // conn->memAsk<node>("hello" + std::to_string(id), 100);
    // for (int i = 0; i < 10; i ++) {
    //     conn->memWrite<node>("hello" + std::to_string(id), node(i, i + 1), i);
    //     std::cout << "client> SET hello" << std::to_string(id) << "[" << i << "] = {" << i << ", " << i + 1 << "}\n";
    // }
    
    /* Get array<node> */
    for (int i = 0; i < 10; i ++) {
        node rd = conn->memRead<node>("hello" + std::to_string(id), i);
        std::cout << "client> GET hello" << std::to_string(id) << "[" << i << "]\n";
        std::cout << "server> {" << rd.x << ", " << rd.y << "}\n";
    }

    conn->Exit();
    exit(0);
}

int main (int argc, char* argv[]) {
    // for (int i = 0; i < 4; i ++) {
    //     int id = fork();
    //     if (id == 0) {
    //         
    //     } else {
    //         sonWork(i);
    //     }
    // }
    // while (-1 != wait(NULL)) {}
    // return 0;
    sonWork(0);
}
