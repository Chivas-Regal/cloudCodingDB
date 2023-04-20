#ifndef _MEMLIST_WF_H_
#define _MEMLIST_WF_H_

#include "memlistnode.h"
#include "memlist.h"

#include <cstdint>

class MemList_WF : public MemList {
public:
    MemList_WF () = delete;
    MemList_WF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_WF() override = default;

    void* allocate (ssize_t size) override;
};

#endif