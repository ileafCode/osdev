#pragma once
#include <stdint.h>
#include <stddef.h>

#define HEAP_IMPL 0

#if HEAP_IMPL == 0

struct HeapSegHdr
{
    size_t length;
    HeapSegHdr* next;
    HeapSegHdr* last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr* Split(size_t splitLength);
};

void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void free(void* address);

void ExpandHeap(size_t length);

#elif HEAP_IMPL == 1

struct Node {
    size_t size;
    Node *next;
} __attribute__((packed));

void init_heap(void* heapAddress);

void* malloc(size_t size);
void free(void* address);

#else

struct Node {
    size_t size;
    uint8_t free;
    Node *next;
} __attribute__((packed));

void* malloc(size_t size);
void free(void* address);

#endif

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}

inline void operator delete(void* p) {free(p);}
inline void operator delete[](void* p) {free(p);}