# include <iostream>
# include <map>
#include <unistd.h>
#include <sys/socket.h>

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
    for (int i = 0; i < s.size(); i ++) {
        buf[i] = s[i];
    }
    buf[s.size()] = '\0';
    if (send(sockfd, &buf, 1024, 0) == -1) {
        std::cout << "error: send\n";
    }
}
