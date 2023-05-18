#pragma once

#include <iostream>
#include <unistd.h>

/* 判断本机字节序 */
int BitHighLow ();

/* 报错封装 */
void errif (bool, const char*);

/* 网络发送字符串 */
void sWrite (int sockfd, const std::string &s);

/* 网络读取字符串 */
std::string sRead (int sockfd);