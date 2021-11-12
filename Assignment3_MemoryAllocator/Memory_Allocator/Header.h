#ifndef FREELISTALLOCATOR_H
#define FREELISTALLOCATOR_H

#include "MemoryAllocator.h"
#include "SinglyLinkedList.h"

class HeapManager : public MemoryAllocator {
public:
    enum PlacementPolicy {
        FIND_FIRST,
        FIND_BEST
    };

private:
    struct FreeHeader {
        std::size_t blockSize;
    };
    struct AllocationHeader {
        std::size_t blockSize;
        char padding;
    };

    typedef SinglyLinkedList<FreeHeader>::Node Node;


    void* m_start_ptr = nullptr;
    PlacementPolicy m_pPolicy;
    SinglyLinkedList<FreeHeader> m_freeList;

public:
    HeapManager(const std::size_t totalSize, const PlacementPolicy pPolicy);

    virtual ~HeapManager();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

    virtual void Free(void* ptr) override;

    virtual void Init() override;

    virtual void Reset();
private:
    HeapManager(HeapManager& freeListAllocator);

    void Coalescence(Node* prevBlock, Node* freeBlock);

    void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
    //void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
    void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
};

#endif /* FREELISTALLOCATOR_H */
