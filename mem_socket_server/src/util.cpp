# include <iostream>
# include <map>
# include <cstring>
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