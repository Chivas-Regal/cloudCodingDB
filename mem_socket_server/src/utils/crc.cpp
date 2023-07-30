#include "crc.h"

uint32_t crc32 (uint8_t* s, size_t length) {
    uint32_t crc = 0xfffffff;
    while (length --) {
        crc = crc ^ *s++;
        for (int32_t i = 0; i < 8; i ++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}