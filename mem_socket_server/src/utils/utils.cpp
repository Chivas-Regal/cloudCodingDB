#include <iostream>
#include <map>
#include <cstring>
#include <assert.h>
#include "utils.h"

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

void setFixed32 (std::string* dst, uint32_t value) {
    uint8_t buffer[4];
    for (int i = 0; i < 4; i ++) {
        buffer[i] = static_cast<uint8_t>(value);
        value >>= 8;
    }
    dst->append((char*)buffer, 4);
}

void setFixed64 (std::string* dst, uint64_t value) {
    uint8_t buffer[8];
    for (int i = 0; i < 8; i ++) {
        buffer[i] = static_cast<uint8_t>(value);
        value >>= 8;
    }
    dst->append((char*)buffer, 8);
}

void setVarint32 (std::string* dst, uint32_t value) {
    uint8_t buffer[5]; 
    int offset = 0;
    while (value >= 128) {
        buffer[offset ++] = static_cast<uint8_t>(value | 128);
        value >>= 7;
    }
    buffer[offset ++] = static_cast<uint8_t>(value);
    dst->append((char*)buffer, offset);
}

void setVarint64 (std::string* dst, uint64_t value) {
    uint8_t buffer[10]; 
    int offset = 0;
    while (value >= 128) {
        buffer[offset ++] = static_cast<uint8_t>(value | 128);
        value >>= 7;
    }
    buffer[offset ++] = static_cast<uint8_t>(value);
    dst->append((char*)buffer, offset);
}

uint32_t getFixed32 (Slice& src) {
    assert(src.size() >= 4);
    uint32_t ret = 0;
    uint32_t bit = 1;
    for (int i = 0; i < 4; i ++) {
        char c = src[i];
        ret += uint32_t(*(uint8_t*)&c) * bit;
        bit <<= 8;
    }
    src.remove_prefix(4);
    return ret;
}

uint64_t getFixed64 (Slice& src) {
    assert(src.size() >= 8);
    uint64_t ret = 0;
    uint64_t bit = 1;
    for (int i = 0; i < 8; i ++) {
        char c = src[i];
        ret += uint64_t(*(uint8_t*)&c) * bit;
        bit <<= 8;
    }
    src.remove_prefix(8);
    return ret;
}
uint32_t getVarint32 (Slice& src) {
    uint32_t ret = 0;
    uint32_t bit = 1;
    for (int i = 0; i < 5; i ++) {
        char c = src[i];
        int x = *(uint8_t*)&c;
        ret += (x & 127) * bit;
        if (!(x & 128)) {
            src.remove_prefix(i + 1);
            return ret;
        }
        bit <<= 7;
    }
    return UINT32_MAX;
}
uint64_t getVarint64 (Slice& src) {
    uint64_t ret = 0;
    uint64_t bit = 1;
    for (int i = 0; i < 10; i ++) {
        char c = src[i];
        int x = *(uint8_t*)&c;
        ret += (x & 127) * bit;
        if (!(x & 128)) {
            src.remove_prefix(i + 1);
            return ret;
        }
        bit <<= 7;
    }
    return UINT64_MAX;
}

uint32_t getFixed32(Slice&& src) {
    Slice slice = src;
    return getFixed32(slice);
}

uint64_t getFixed64(Slice&& src) {
    Slice slice = src;
    return getFixed64(slice);
}

uint32_t getVarint32(Slice&& src) {
    Slice slice = src;
    return getVarint32(slice);
}

uint64_t getVarint64(Slice&& src) {
    Slice slice = src;
    return getVarint64(slice);
}