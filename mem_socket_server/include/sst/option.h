#ifndef _INCLUDE_SST_OPTION_H_
#define _INCLUDE_SST_OPTION_H_

#include <iostream>

class Option {
public:
    static const uint64_t magic_number = 1411390466;
    
    static const size_t CACHE_SIZE = 10;

    static const size_t SST_SIZE = 100;
};

#endif