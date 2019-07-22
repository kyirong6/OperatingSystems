#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "kallocator.h"

#define MAXSIZE 100


struct Chunk {
    int size;
    void* start;
};


struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    struct Chunk mfree[MAXSIZE];
    struct Chunk mallo[MAXSIZE];
    int sizeF;
    int sizeA;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
};

struct KAllocator kallocator;



void calculateAlloc()
{
    struct Chunk temp[MAXSIZE];
    for(int i=0;i<MAXSIZE;i++)
    {
        temp[i].start=kallocator.mallo[i].start;
        temp[i].size=kallocator.mallo[i].size;
    }   
    kallocator.sizeA=0;
    for(int i=0;i<MAXSIZE;i++)
    {
        kallocator.mallo[i].start=NULL;
        kallocator.mallo[i].size=-1;
    }
        for(int i=0;i<MAXSIZE;i++)
    {
        if(temp[i].start!=NULL)
        {
            kallocator.mallo[kallocator.sizeA].start=temp[i].start;
            kallocator.mallo[kallocator.sizeA].size=temp[i].size;
            kallocator.sizeA++;
        }
    }
}


void calculateFree()
{
    intptr_t length;
    kallocator.sizeF=0;
    void* s=kallocator.memory;
    for(int i=0;i<MAXSIZE;i++)
    {
        kallocator.mfree[i].start=NULL;
        kallocator.mfree[i].size=-1;
    }
    for(int i=0;i<kallocator.sizeA;i++)
    {
        length= (intptr_t)kallocator.mallo[i].start-(intptr_t)s;
        if(length>0)
        {
            kallocator.mfree[kallocator.sizeF].start=s;
            kallocator.mfree[kallocator.sizeF].size=length;
            kallocator.sizeF++;
        }
        s=kallocator.mallo[i].start+kallocator.mallo[i].size;
    }
    length=(intptr_t)kallocator.memory+(intptr_t)kallocator.size-(intptr_t)s;
    if(length>0)
    {
        kallocator.mfree[kallocator.sizeF].start=s;
        kallocator.mfree[kallocator.sizeF].size=length;
        kallocator.sizeF++;
    }
}


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);
    for(int i=0;i<MAXSIZE;i++)
    {
        kallocator.mfree[i].start=NULL;
        kallocator.mallo[i].start=NULL;
        kallocator.mfree[i].size=-1;
        kallocator.mallo[i].size=-1;
    }
    kallocator.sizeA=0;
    kallocator.sizeF=0;
    kallocator.mfree[0].start=kallocator.memory;
    kallocator.mfree[0].size=kallocator.size;
    kallocator.sizeF++;
    // Add some other initialization 
}

void destroy_allocator() {
    free(kallocator.memory);

    // free other dynamic allocated memory to avoid memory leak
}

void* kalloc(int _size) {
    void* ptr = NULL;
    if(kallocator.aalgorithm==FIRST_FIT)
    {
        for(int i=0;i<MAXSIZE;i++)
        {
            if(kallocator.mfree[i].size>_size)
            {
                kallocator.mallo[kallocator.sizeA].start=kallocator.mfree[i].start;
                kallocator.mallo[kallocator.sizeA].size=_size;
                kallocator.sizeA++;
                calculateFree();
                //printf("Fround first_fit %d \n",kallocator.mallo[kallocator.sizeA-1].start);
                return kallocator.mallo[kallocator.sizeA-1].start;
            }
        }
        return NULL;
    }
    else if(kallocator.aalgorithm==BEST_FIT)
    {
        int best=-1;
        for(int i=0;i<MAXSIZE;i++)
        {
            if(kallocator.mfree[i].size>_size)
            {
                if(best==-1)
                    best=i;
                else if(kallocator.mfree[i].size<kallocator.mfree[best].size)
                    best=i;
            }
        }
        if(best==-1)
            return NULL;
        kallocator.mallo[kallocator.sizeA].start=kallocator.mfree[best].start;
        kallocator.mallo[kallocator.sizeA].size=_size;
        kallocator.sizeA++;
        calculateFree();
        return kallocator.mallo[kallocator.sizeA-1].start;
    }
    else if(kallocator.aalgorithm==WORST_FIT)
    {
        int worst=-1;
        for(int i=0;i<MAXSIZE;i++)
        {
            if(kallocator.mfree[i].size>_size)
            {
                if(worst==-1)
                    worst=i;
                else if(kallocator.mfree[i].size>kallocator.mfree[worst].size)
                    worst=i;
            }
        }
        if(worst==-1)
            return NULL;
        kallocator.mallo[kallocator.sizeA].start=kallocator.mfree[worst].start;
        kallocator.mallo[kallocator.sizeA].size=_size;
        kallocator.sizeA++;
        calculateFree();
        return kallocator.mallo[kallocator.sizeA-1].start;
    }
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory
    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);
    for(int i=0;i<MAXSIZE;i++)
    {
        if(kallocator.mallo[i].start==_ptr)
        {
            kallocator.mallo[i].start=NULL;
            kallocator.mallo[i].size=-1;
        }
    }
    calculateAlloc();
    calculateFree();
}



int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;
    // int j=0;
    // int k=0;
    // for(int i=0;i<MAXSIZE*2;i++)ss
    // {
    //     if(j<kallocator.sizeA)
    //     {
    //         if(k<kallocator.sizeF)
    //         {
    //             if((intptr_t)kallocator.mallo[j].start<(intptr_t)kallocator.mfree[k].start)
    //                 _before[i]=kallocator.mallo[j++].start;
    //             else
    //                 _before[i]=kallocator.mfree[k++].start;
    //         }
    //         else
    //         {
    //             _before[i]=kallocator.mallo[j++].start;
    //         }
    //     }
    //     else if(k<kallocator.sizeF)
    //         _before[i]=kallocator.mfree[k++].start;
    //     else
    //         break;
    // }
    // compacted_size+=kallocator.sizeA;
    // compacted_size+=kallocator.sizeF;

    for(int i=0;i<kallocator.sizeA;i++)
    {
        _before[i]=kallocator.mallo[i].start;
    }


    void* s=kallocator.memory;
    for(int i=0;i<kallocator.sizeA;i++)
    {
        kallocator.mallo[i].start=s;
        s+=kallocator.mallo[i].size;
    }
    calculateFree();

    for(int i=0;i<kallocator.sizeA;i++)
    {
        _after[i]=kallocator.mallo[i].start;
    }

    // j=0;
    // k=0;
    // for(int i=0;i<MAXSIZE*2;i++)
    // {
    //     if(j<kallocator.sizeA)
    //     {
    //         if(k<kallocator.sizeF)
    //         {
    //             if((intptr_t)kallocator.mallo[j].start<(intptr_t)kallocator.mfree[k].start)
    //                 _after[i]=kallocator.mallo[j++].start;
    //             else
    //                 _after[i]=kallocator.mfree[k++].start;
    //         }
    //         else
    //         {
    //             _after[i]=kallocator.mallo[j++].start;
    //         }
    //     }
    //     else if(k<kallocator.sizeF)
    //         _after[i]=kallocator.mfree[k++].start;
    //     else
    //         break;
    // }

    // compacted_size+=kallocator.sizeA;
    // compacted_size+=kallocator.sizeF;

    // compact allocated memory
    // update _before, _after and compacted_size

    return kallocator.sizeA;
}

int available_memory() {
    int available_memory_size = 0;
    // Calculate available memory size
    for(int i=0;i<MAXSIZE;i++)
    {
        if(kallocator.mfree[i].size!=-1)
        {
            available_memory_size+=kallocator.mfree[i].size;
        }
    }
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = kallocator.size;
    int largest_free_chunk_size = 0;

    for(int i=0;i<MAXSIZE;i++)
    {
        if(kallocator.mfree[i].size!=-1)
        {
            printf("free %d: %p, %d\n",i,kallocator.mfree[i].start,kallocator.mfree[i].size);
            free_size+=kallocator.mfree[i].size;
            free_chunks+=1;
            if(kallocator.mfree[i].size<smallest_free_chunk_size)
                smallest_free_chunk_size=kallocator.mfree[i].size;
            if(kallocator.mfree[i].size>largest_free_chunk_size)
                largest_free_chunk_size=kallocator.mfree[i].size;
        }
        if(kallocator.mallo[i].size!=-1)
        {
            printf("alloc %d: %p, %d\n",i,kallocator.mallo[i].start,kallocator.mallo[i].size);
            allocated_size+=kallocator.mallo[i].size;
            allocated_chunks+=1;
        }
    }



    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}



