#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

// @return
//  - 1: big
//  - 2: small
int BitHighLow ();

void errif (bool, const char*);

std::string sRead (int sockfd);
void sWrite (int sockfd, const std::string &s);