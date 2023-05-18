# include <iostream>
# include <map>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
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

/**
 * @brief 网络发送字符串
 * 
 * @param sockfd 目标套接字
 * @param s      待发送字符串
 * 
 * @details 将 s 放入 char buf[] 中，结尾处设置为 '\n'
 */
void sWrite (int sockfd, const std::string &s) {
    char buf[128]; 
    bzero(buf, sizeof(buf));
    for (int i = 0; i < s.size(); i ++) {
        buf[i] = s[i];
    }
    buf[s.size()] = '\n';
    send(sockfd, buf, sizeof(buf), 0);
}

/**
 * @brief 网络读取字符串
 * 
 * @param sockfd 目标套接字
 * 
 * @return std::string 返回读取后的字符串
 */
std::string sRead (int sockfd) {
    char buf[128]; bzero(buf, sizeof(buf));
    if (recv(sockfd, buf, sizeof(buf), 0) == -1) {
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
