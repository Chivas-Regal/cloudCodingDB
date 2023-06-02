#include <iostream>
#include <map>
#include <cstring>
#include "../include/util.h"

/**
 * @brief 判断本机字节序
 * 
 * @return int 
 *      - 1: 大端字节序
 *      - 2: 小端字节序
 */
int BitHighLow () {
	int a = 0x1234;
	if (static_cast<char>(a) == 0x12) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * @brief 报错封装
 * 
 * @details 若报错直接退出
 * 
 * @param flag 成立检测
 * @param msg  报错信息
 */
void errif (bool flag, const char* msg) {
	if (flag) {
		std::cout << msg << std::endl;
		exit(EXIT_FAILURE);
	}
}