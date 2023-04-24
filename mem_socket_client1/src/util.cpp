# include <iostream>
# include <map>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include "../include/util.h"

int BitHighLow () {
	int a = 0x1234;
	if (static_cast<char>(a) == 0x12) {
		return 1;
	} else {
		return 0;
	}
}

void errif (bool flag, const char* msg) {
	if (flag) {
		std::cout << msg << std::endl;
		exit(EXIT_FAILURE);
	}
}

void sWrite (int sockfd, const std::string &s) {
    char buf[1024]; 
    bzero(buf, sizeof(buf));
    for (int i = 0; i < s.size(); i ++) {
        buf[i] = s[i];
    }
    buf[s.size()] = '\n';
    send(sockfd, &buf, sizeof(buf), 0);
}

std::string sRead (int sockfd) {
    char buf[1024]; bzero(buf, sizeof(buf));
    if (recv(sockfd, &buf, sizeof(buf), 0) == -1) {
        std::cout << "error: recv\n";
        return std::string("e");
    }
    std::string ret;
    for (int i = 0; buf[i] != '\n'; i ++) {
        if (buf[i] != '\0') 
            ret += buf[i];
    }
    return ret;
}
