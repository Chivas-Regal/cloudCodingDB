#include <iostream>
#include <cstring>
#include "utils.h"
#include "table.h"
#include <fstream>
#include <assert.h>
#include <unordered_map>

std::unordered_map<std::string, std::string> mp;

int main () {
    srand(time(NULL));
    
    TableBuilder tabbd(64);
    for (int i = 0; i < 10000; i ++) {
        int x = rand() % 1000000;
        while (x == 0)
            x = rand() % 1000000;
        std::string key;
        while (x) {
            key += x % 26 + 'a';
            x /= 26;
        }
        if (mp.count(key)) continue;
        int y = rand() % 1000000;
        while (y == 0)
            y = rand () % 1000000;
        std::string value;
        while (y) {
            value += y % 26 + 'a';
            y /= 26;
        }
        mp[key] = value;
        tabbd.addEntry(key, value);
    }    
    tabbd.finish();
    tabbd.flush("data.sst");

    Table reader("data.sst");
    for (auto [key, value] : mp) {
        assert(reader.getValue(key) == value);
    }
}