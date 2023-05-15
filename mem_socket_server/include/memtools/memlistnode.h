#ifndef _MEMLIST_NODE_H_
#define _MEMLIST_NODE_H_

#include <cstdio>

class MemListNode {
public:
    MemListNode *next;

    MemListNode ();
    MemListNode (ssize_t _size, MemListNode *_next);
    ~MemListNode () = default;

    [[nodiscard]] const void *getAddress () const;
    [[nodiscard]] ssize_t  getSize () const;
private:
    ssize_t size;

friend class MemList;
friend class MemList_FF;
friend class MemList_WF;
friend class MemList_BF;
};


#endif