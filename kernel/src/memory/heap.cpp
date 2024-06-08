#include "heap.h"
#include "../stdio/stdio.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"
#include "../scheduling/task/sched.h"

#if HEAP_IMPL == 0

void *heapStart;
void *heapEnd;
HeapSegHdr *LastHdr;

void InitializeHeap(void *heapAddress, size_t pageCount)
{
    void *pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++)
    {
        g_PageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void *)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void *)((size_t)heapStart + heapLength);
    HeapSegHdr *startSeg = (HeapSegHdr *)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    LastHdr = startSeg;
}

void free(void *address)
{
    HeapSegHdr *segment = (HeapSegHdr *)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void *malloc(size_t size)
{
    sched_lock();
    if (size % 0x10 > 0)
    { // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0)
        return NULL;

    HeapSegHdr *currentSeg = (HeapSegHdr *)heapStart;
    while (true)
    {
        if (currentSeg->free)
        {
            if (currentSeg->length > size)
            {
                currentSeg->Split(size);
                currentSeg->free = false;
                sched_unlock();
                return (void *)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
            if (currentSeg->length == size)
            {
                currentSeg->free = false;
                sched_unlock();
                return (void *)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
        }
        if (currentSeg->next == NULL)
            break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size);
}

HeapSegHdr *HeapSegHdr::Split(size_t splitLength)
{
    if (splitLength < 0x10)
        return NULL;
    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegHdr));
    if (splitSegLength < 0x10)
        return NULL;

    HeapSegHdr *newSplitHdr = (HeapSegHdr *)((size_t)this + splitLength + sizeof(HeapSegHdr));
    next->last = newSplitHdr; // Set the next segment's last segment to our new segment

    newSplitHdr->next = next;             // Set the new segment's next segment to out original next segment
    next = newSplitHdr;                   // Set our new segment to the new segment
    newSplitHdr->last = this;             // Set our new segment's last segment to the current segment
    newSplitHdr->length = splitSegLength; // Set the new header's length to the calculated value
    newSplitHdr->free = free;             // make sure the new segment's free is the same as the original
    length = splitLength;                 // set the length of the original segment to its new length
    if (LastHdr == this)
        LastHdr = newSplitHdr;
    return newSplitHdr;
}

void ExpandHeap(size_t length)
{
    if (length % 0x1000)
    {
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHdr *newSegment = (HeapSegHdr *)heapEnd;

    for (size_t i = 0; i < pageCount; i++)
    {
        g_PageTableManager.MapMemory(heapEnd, GlobalAllocator.RequestPage());
        heapEnd = (void *)((size_t)heapEnd + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegHdr);
    newSegment->CombineBackward();
}

void HeapSegHdr::CombineForward()
{
    if (next == NULL)
        return;
    if (!next->free)
        return;

    if (next == LastHdr)
        LastHdr = this;

    if (next->next != NULL)
    {
        next->next->last = this;
    }

    length = length + next->length + sizeof(HeapSegHdr);
}

void HeapSegHdr::CombineBackward()
{
    if (last != NULL && last->free)
        last->CombineForward();
}

#elif HEAP_IMPL == 1

MemorySegmentHeader *FirstFreeMemorySegment;

void init_heap(void *heapAddress, uint64_t pageCount)
{
    void *pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++)
    {
        g_PageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void *)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    FirstFreeMemorySegment = (MemorySegmentHeader *)heapAddress;
	FirstFreeMemorySegment->MemoryLength = heapLength - sizeof(MemorySegmentHeader);
	FirstFreeMemorySegment->NextSegment = 0;
	FirstFreeMemorySegment->PreviousSegment = 0;
	FirstFreeMemorySegment->NextFreeSegment = 0;
	FirstFreeMemorySegment->PreviousFreeSegment = 0;
	FirstFreeMemorySegment->Free = true;
}

void CombineFreeSegments(MemorySegmentHeader *a, MemorySegmentHeader *b)
{
	if (a == 0)
		return;
	if (b == 0)
		return;
	if (a < b)
	{
		a->MemoryLength += b->MemoryLength + sizeof(MemorySegmentHeader);
		a->NextSegment = b->NextSegment;
		a->NextFreeSegment = b->NextFreeSegment;
		b->NextSegment->PreviousSegment = a;
		b->NextSegment->PreviousFreeSegment = a;
		b->NextFreeSegment->PreviousFreeSegment = a;
	}
	else
	{
		b->MemoryLength += a->MemoryLength + sizeof(MemorySegmentHeader);
		b->NextSegment = a->NextSegment;
		b->NextFreeSegment = a->NextFreeSegment;
		a->NextSegment->PreviousSegment = b;
		a->NextSegment->PreviousFreeSegment = b;
		a->NextFreeSegment->PreviousFreeSegment = b;
	}
}

void *malloc(size_t size)
{
    uint64_t remainder = size % 8;
    size -= remainder;
    if (remainder != 0)
        size += 8;

    MemorySegmentHeader *currentMemorySegment = FirstFreeMemorySegment;

    while (true)
    {
        if (currentMemorySegment->MemoryLength >= size)
        {
            if (currentMemorySegment->MemoryLength > size + sizeof(MemorySegmentHeader))
            {
                MemorySegmentHeader *newSegmentHeader = (MemorySegmentHeader *)((uint64_t)currentMemorySegment + sizeof(MemorySegmentHeader) + size);

                newSegmentHeader->Free = true;
                newSegmentHeader->MemoryLength = ((uint64_t)currentMemorySegment->MemoryLength) - (sizeof(MemorySegmentHeader) + size);
                newSegmentHeader->NextFreeSegment = currentMemorySegment->NextFreeSegment;
                newSegmentHeader->NextSegment = currentMemorySegment->NextSegment;
                newSegmentHeader->PreviousSegment = currentMemorySegment;
                newSegmentHeader->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;

                currentMemorySegment->NextFreeSegment = newSegmentHeader;
                currentMemorySegment->NextSegment = newSegmentHeader;
                currentMemorySegment->MemoryLength = size;
            }
            if (currentMemorySegment == FirstFreeMemorySegment)
            {
                FirstFreeMemorySegment = currentMemorySegment->NextFreeSegment;
            }
            currentMemorySegment->Free = false;

            if (currentMemorySegment->PreviousFreeSegment != 0)
                currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
            if (currentMemorySegment->NextFreeSegment != 0)
                currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
            if (currentMemorySegment->PreviousSegment != 0)
                currentMemorySegment->PreviousSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
            if (currentMemorySegment->NextSegment != 0)
                currentMemorySegment->NextSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
            
            return currentMemorySegment + 1;
        }
        if (currentMemorySegment->NextFreeSegment == 0)
        {
            return 0; // No memory remaining
        }
        currentMemorySegment = currentMemorySegment->NextFreeSegment;
    }
    return 0; // we should never get here
}

// Function to free memory allocated by malloc
void free(void *ptr)
{
    MemorySegmentHeader *currentMemorySegment;

	AlignedMemorySegmentHeader *AMSH = (AlignedMemorySegmentHeader *)ptr - 1;
	if (AMSH->IsAligned)
	{
		currentMemorySegment = (MemorySegmentHeader *)(uint64_t)AMSH->MemorySegmentHeaderAddress;
	}
	else
	{
		currentMemorySegment = ((MemorySegmentHeader *)ptr) - 1;
	}
	currentMemorySegment->Free = true;

	if (currentMemorySegment < FirstFreeMemorySegment)
		FirstFreeMemorySegment = currentMemorySegment;

	if (currentMemorySegment->NextFreeSegment != 0)
	{
		if (currentMemorySegment->NextFreeSegment->PreviousFreeSegment < currentMemorySegment)
			currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->PreviousFreeSegment != 0)
	{
		if (currentMemorySegment->PreviousFreeSegment->NextFreeSegment > currentMemorySegment)
			currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->NextSegment != 0)
	{
		currentMemorySegment->NextSegment->PreviousSegment = currentMemorySegment;
		if (currentMemorySegment->NextSegment->Free)
			CombineFreeSegments(currentMemorySegment, currentMemorySegment->NextSegment);
	}
	if (currentMemorySegment->PreviousSegment != 0)
	{
		currentMemorySegment->PreviousSegment->NextSegment = currentMemorySegment;
		if (currentMemorySegment->PreviousSegment->Free)
			CombineFreeSegments(currentMemorySegment, currentMemorySegment->PreviousSegment);
	}
}

#endif