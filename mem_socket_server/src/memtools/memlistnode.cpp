#include "../../include/memtools/memlistnode.h"

MemListNode::MemListNode ():
    next(nullptr),
    size(0)
{}

MemListNode::MemListNode (ssize_t _size, MemListNode *_next):
    size(_size),
    next(_next)
{}

[[nodiscard]] const void* MemListNode::getAddress () const {
    return this;
}

[[nodiscard]] ssize_t  MemListNode::getSize () const {
    return size;
}