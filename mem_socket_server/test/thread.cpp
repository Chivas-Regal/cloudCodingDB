#include "../include/threads/threadpool.h"
#include <assert.h>

int addFunc (int a, int b) {
    int sum = 0;
    for (int i = a; i <= b; i ++)
        sum += i;
    return sum;
}

int main () {
    std::vector<std::future<int>> ret;
    ThreadsPool* thp = new ThreadsPool();
    for (int i = 0; i < 10; i ++) {
        ret.push_back(thp->commit(std::move(addFunc), i, i + 10));
    }
    for (int i = 0; i < 10; i ++) {
        int sum = 0;
        for (int j = i; j <= i + 10; j ++) {
            sum += j;
        }
        assert(sum == ret[i].get());
    }
}