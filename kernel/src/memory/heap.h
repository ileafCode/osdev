#pragma once
#include <stdint.h>
#include <stddef.h>

#define HEAP_IMPL 1

#if HEAP_IMPL == 0

struct HeapSegHdr
{
    size_t length;
    HeapSegHdr *next;
    HeapSegHdr *last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr *Split(size_t splitLength);
};

void InitializeHeap(void *heapAddress, size_t pageCount);

void *malloc(size_t size);
void free(void *address);

void ExpandHeap(size_t length);

#elif HEAP_IMPL == 1

struct MemorySegmentHeader
{
    uint64_t MemoryLength;
    MemorySegmentHeader *NextSegment;
    MemorySegmentHeader *PreviousSegment;
    MemorySegmentHeader *NextFreeSegment;
    MemorySegmentHeader *PreviousFreeSegment;
    bool Free;
} __attribute__((packed));

struct AlignedMemorySegmentHeader
{
    uint64_t MemorySegmentHeaderAddress : 63;
    bool IsAligned : 1;
} __attribute__((packed));

void init_heap(void *heapAddress, uint64_t pageCount);

void *malloc(size_t size);
void free(void *address);

#endif

inline void *operator new(size_t size) { return malloc(size); }
inline void *operator new[](size_t size) { return malloc(size); }

inline void operator delete(void *p) { free(p); }
inline void operator delete[](void *p) { free(p); }