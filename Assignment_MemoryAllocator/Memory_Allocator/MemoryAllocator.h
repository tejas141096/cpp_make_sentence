#pragma once
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t

class MemoryAllocator {
protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;
public:

    MemoryAllocator(const std::size_t totalSize) : m_totalSize{ totalSize }, m_used{ 0 }, m_peak{ 0 } { }

    virtual ~MemoryAllocator() { m_totalSize = 0; }

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void Init() = 0;
};

#endif /* ALLOCATOR_H */
