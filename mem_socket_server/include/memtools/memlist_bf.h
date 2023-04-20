#ifndef _MEMLIST_BF_H_
#define _MEMLIST_BF_H_

#include "memlistnode.h"
#include "memlist.h"

class MemList_BF : public MemList {
public:
    MemList_BF () = delete;
    MemList_BF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_BF() override = default;

    void* allocate (ssize_t size) override;
};

#endif