#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

/* 判断本机字节序 */
int BitHighLow ();

/* 报错封装 */
void errif (bool, const char*);