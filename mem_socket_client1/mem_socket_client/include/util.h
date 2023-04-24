#pragma once

#include <iostream>
#include <unistd.h>

// @return
//  - 1: big
//  - 2: small
int BitHighLow ();

void errif (bool, const char*);


void sWrite (int sockfd, const std::string &s);

std::string sRead (int sockfd);