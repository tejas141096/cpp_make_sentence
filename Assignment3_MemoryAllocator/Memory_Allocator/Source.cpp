#include "Header.h"
#include "Utils.h"
#include <stdlib.h>
#include <limits>
#include <algorithm>


//#include "HeapManagerProxy.h"
#include <Windows.h>

#include <assert.h>
#include <vector>

#define SUPPORTS_ALIGNMENT
#define SUPPORTS_SHOWFREEBLOCKS
#define SUPPORTS_SHOWOUTSTANDINGALLOCATIONS

#ifdef _DEBUG
#include <iostream>
#endif

HeapManager::HeapManager(const std::size_t totalSize, const PlacementPolicy pPolicy)
    : MemoryAllocator(totalSize) {
    m_pPolicy = pPolicy;
}

void HeapManager::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);

    this->Reset();
}

HeapManager::~HeapManager() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

void* HeapManager::Allocate(const std::size_t size, const std::size_t alignment) {
    const std::size_t allocationHeaderSize = sizeof(HeapManager::AllocationHeader);
    const std::size_t freeHeaderSize = sizeof(HeapManager::FreeHeader);
    assert("Allocation size must be bigger" && size >= sizeof(Node));
    assert("Alignment must be 4 at least" && alignment >= 4);

    // Search through the free list for a free block that has enough space to allocate our data
    std::size_t padding;
    Node* affectedNode,
        * previousNode;
    this->Find(size, alignment, padding, previousNode, affectedNode);
	assert(affectedNode != nullptr && "Not enough memory");


    const std::size_t alignmentPadding = padding - allocationHeaderSize;
    const std::size_t requiredSize = size + padding;

    const std::size_t rest = affectedNode->data.blockSize - requiredSize;

    if (rest > 0) {
        // We have to split the block into the data block and a free block of size 'rest'
        Node* newFreeNode = (Node*)((std::size_t)affectedNode + requiredSize);
        newFreeNode->data.blockSize = rest;
        m_freeList.insert(affectedNode, newFreeNode);
    }
    m_freeList.remove(previousNode, affectedNode);

    // Setup data block
    const std::size_t headerAddress = (std::size_t)affectedNode + alignmentPadding;
    const std::size_t dataAddress = headerAddress + allocationHeaderSize;
    ((HeapManager::AllocationHeader*)headerAddress)->blockSize = requiredSize;
    ((HeapManager::AllocationHeader*)headerAddress)->padding = alignmentPadding;

    m_used += requiredSize;
    m_peak = max(m_peak, m_used);

#ifdef _DEBUG
    std::cout << "A" << "\t@H " << (void*)headerAddress << "\tD@ " << (void*)dataAddress << "\tS " << ((HeapManager::AllocationHeader*)headerAddress)->blockSize << "\tAP " << alignmentPadding << "\tP " << padding << "\tM " << m_used << "\tR " << rest << std::endl;
#endif

    return (void*)dataAddress;
}

void HeapManager::Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode) {
    switch (m_pPolicy) {
    case FIND_FIRST:
        FindFirst(size, alignment, padding, previousNode, foundNode);
        break;
    //case FIND_BEST:
        //FindBest(size, alignment, padding, previousNode, foundNode);
        //break;
    }
}

void HeapManager::FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode) {
    //Iterate list and return the first free block with a size >= than given size
    Node* it = m_freeList.head,
        * itPrev = nullptr;

    while (it != nullptr) {
        padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof(HeapManager::AllocationHeader));
        const std::size_t requiredSpace = size + padding;
        if (it->data.blockSize >= requiredSpace) {
            break;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = itPrev;
    foundNode = it;
}

//void HeapManager::FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode) {
//    // Iterate WHOLE list keeping a pointer to the best fit
//    std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
//    Node* bestBlock = nullptr;
//    Node* it = m_freeList.head,
//        * itPrev = nullptr;
//    while (it != nullptr) {
//        padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof(HeapManager::AllocationHeader));
//        const std::size_t requiredSpace = size + padding;
//        if (it->data.blockSize >= requiredSpace && (it->data.blockSize - requiredSpace < smallestDiff)) {
//            bestBlock = it;
//        }
//        itPrev = it;
//        it = it->next;
//    }
//    previousNode = itPrev;
//    foundNode = bestBlock;
//}

void HeapManager::Free(void* ptr) {
    // Insert it in a sorted position by the address number
    const std::size_t currentAddress = (std::size_t)ptr;
    const std::size_t headerAddress = currentAddress - sizeof(HeapManager::AllocationHeader);
    const HeapManager::AllocationHeader* allocationHeader{ (HeapManager::AllocationHeader*)headerAddress };

    Node* freeNode = (Node*)(headerAddress);
    freeNode->data.blockSize = allocationHeader->blockSize + allocationHeader->padding;
    freeNode->next = nullptr;

    Node* it = m_freeList.head;
    Node* itPrev = nullptr;
    while (it != nullptr) {
        if (ptr < it) {
            m_freeList.insert(itPrev, freeNode);
            break;
        }
        itPrev = it;
        it = it->next;
    }

    m_used -= freeNode->data.blockSize;

    // Merge contiguous nodes
    Coalescence(itPrev, freeNode);

#ifdef _DEBUG
    std::cout << "F" << "\t@ptr " << ptr << "\tH@ " << (void*)freeNode << "\tS " << freeNode->data.blockSize << "\tM " << m_used << std::endl;
#endif
}

void HeapManager::Coalescence(Node* previousNode, Node* freeNode) {
    if (freeNode->next != nullptr &&
        (std::size_t)freeNode + freeNode->data.blockSize == (std::size_t)freeNode->next) {
        freeNode->data.blockSize += freeNode->next->data.blockSize;
        m_freeList.remove(freeNode, freeNode->next);
#ifdef _DEBUG
        std::cout << "\tMerging(n) " << (void*)freeNode << " & " << (void*)freeNode->next << "\tS " << freeNode->data.blockSize << std::endl;
#endif
    }

    if (previousNode != nullptr &&
        (std::size_t)previousNode + previousNode->data.blockSize == (std::size_t)freeNode) {
        previousNode->data.blockSize += freeNode->data.blockSize;
        m_freeList.remove(previousNode, freeNode);
#ifdef _DEBUG
        std::cout << "\tMerging(p) " << (void*)previousNode << " & " << (void*)freeNode << "\tS " << previousNode->data.blockSize << std::endl;
#endif
    }
}

void HeapManager::Reset() {
    m_used = 0;
    m_peak = 0;
    Node* firstNode = (Node*)m_start_ptr;
    firstNode->data.blockSize = m_totalSize;
    firstNode->next = nullptr;
    m_freeList.head = nullptr;
    m_freeList.insert(nullptr, firstNode);
}




bool HeapManager_UnitTest()
{
	const size_t 		sizeHeap = 1024 * 1024;
	//const unsigned int 	numDescriptors = 2048;

#ifdef USE_HEAP_ALLOC
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
#else
	// Get SYSTEM_INFO, which includes the memory page size
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	// round our size to a multiple of memory page size
	assert(SysInfo.dwPageSize > 0);
	size_t sizeHeapInPageMultiples = SysInfo.dwPageSize * ((sizeHeap + SysInfo.dwPageSize) / SysInfo.dwPageSize);
	void* pHeapMemory = VirtualAlloc(NULL, sizeHeapInPageMultiples, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
	assert(pHeapMemory);

	// Create a heap manager for my test heap.
	HeapManager* pHeapManager = new HeapManager(sizeHeap, HeapManager::PlacementPolicy::FIND_FIRST);
	assert(pHeapManager);

	if (pHeapManager == nullptr)
		return false;

#ifdef TEST_SINGLE_LARGE_ALLOCATION
	// This is a test I wrote to check to see if using the whole block if it was almost consumed by 
	// an allocation worked. Also helped test my ShowFreeBlocks() and ShowOutstandingAllocations().
	{
#ifdef SUPPORTS_SHOWFREEBLOCKS
		ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS

		size_t largestBeforeAlloc = GetLargestFreeBlock(pHeapManager);
		void* pPtr = alloc(pHeapManager, largestBeforeAlloc - HeapManager::s_MinumumToLeave);

		if (pPtr)
		{
#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
			printf("After large allocation:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
			ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS
#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			printf("\n");
#endif

			size_t largestAfterAlloc = GetLargestFreeBlock(pHeapManager);
			bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
			assert(success);

			success = free(pHeapManager, pPtr);
			assert(success);

			Collect(pHeapManager);

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
			printf("After freeing allocation and garbage collection:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
			ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS
#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			printf("\n");
#endif

			size_t largestAfterCollect = GetLargestFreeBlock(pHeapManager);
		}
	}
#endif

	std::vector<void*> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do
	{
		const size_t		maxTestAllocationSize = 1024;

		size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

#ifdef SUPPORTS_ALIGNMENT
		// pick an alignment
		const uint16_t	alignments[] = { 4, 8, 16, 32, 64 };

		const uint16_t	index = rand() % (sizeof(alignments) / sizeof(alignments[0]));

		const uint16_t	alignment = alignments[index];

		void* pPtr = pHeapManager->Allocate(sizeAlloc, alignment);

		// check that the returned address has the requested alignment
		assert((reinterpret_cast<uintptr_t>(pPtr) & (alignment - 1)) == 0);
#else
		void* pPtr = alloc(pHeapManager, sizeAlloc);
#endif // SUPPORT_ALIGNMENT

//		// if allocation failed see if garbage collecting will create a large enough block
//		if (pPtr == nullptr)
//		{
//			Collect(pHeapManager);
//
//#ifdef SUPPORTS_ALIGNMENT
//			pPtr = alloc(pHeapManager, sizeAlloc, alignment);
//#else
//			pPtr = alloc(pHeapManager, sizeAlloc);
//#endif // SUPPORT_ALIGNMENT
//
//			// if not we're done. go on to cleanup phase of test
//			if (pPtr == nullptr)
//				break;
//		}

		AllocatedAddresses.push_back(pPtr);
		numAllocs++;

		// randomly free and/or garbage collect during allocation phase
		const unsigned int freeAboutEvery = 10;
		const unsigned int garbageCollectAboutEvery = 40;

		if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0))
		{
			void* pPtr = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			//bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
			//assert(success);

			pHeapManager->Free(pPtr);
			//assert(success);

			numFrees++;
		}

		if ((rand() % garbageCollectAboutEvery) == 0)
		{
			//Collect(pHeapManager);

			numCollects++;
		}

	} while (1);

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
	printf("After exhausting allocations:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
	//ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS
#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
	//ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
	printf("\n");
#endif

	// now free those blocks in a random order
	if (!AllocatedAddresses.empty())
	{
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty())
		{
			void* pPtr = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			//bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
			//assert(success);

			pHeapManager->Free(pPtr);
			//assert(success);
		}

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
		printf("After freeing allocations:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
		//ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS

#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		//ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		printf("\n");
#endif

		// do garbage collection
		//Collect(pHeapManager);
		// our heap should be one single block, all the memory it started with

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
		printf("After garbage collection:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
		//ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS

#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		//ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		printf("\n");
#endif

		// do a large test allocation to see if garbage collection worked
		void* pPtr = pHeapManager->Allocate(sizeHeap / 2, 4);
		assert(pPtr);

		if (pPtr)
		{
			//bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
			//assert(success);

			pHeapManager->Free(pPtr);
			//assert(success);

		}
	}

	pHeapManager->Free(pHeapManager);
	pHeapManager = nullptr;

	if (pHeapMemory)
	{
#ifdef USE_HEAP_ALLOC
		HeapFree(GetProcessHeap(), 0, pHeapMemory);
#else
		VirtualFree(pHeapMemory, 0, MEM_RELEASE);
#endif
	}

	// we succeeded
	return true;
}

int main()
{
	bool success = HeapManager_UnitTest();
	assert(success);
	return 0;
}