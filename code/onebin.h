#include <stdio.h>
#include <stdlib.h>

// Heap Size : 256 MB = 268435456 Bytes
#define SIZE 268435456


// Static Allocation : starting address 
char spheap[SIZE];
int blockSize = 0;

//Track Internal Fragmentation
double spaceAllocated = 0;
double spaceRequested = 0;
long requests=0;

//Free list
struct block *available = NULL;

// to free and track available blocks
struct block{
    int address;
    struct block *next;
};

void mem_init(int size){
    blockSize = size;
    int address = 0;
    while(address+blockSize<SIZE){
        struct block *p = (struct block *)malloc(sizeof(struct block));
        p->address = address;
        address += blockSize;
        p->next = available;
        available = p;
    }
}

// Allocate memory
char* mmalloc(size_t reqSize){
    if(reqSize > blockSize || available==NULL){
        printf("\n");
        for(int i=0 ; i<40 ; i++)
            printf("#");
        printf("\n\nINSUFFICIENT STORAGE\n\n");
        printf("\n\nInternal Fragmentation : %.3f \n",(spaceAllocated-spaceRequested)/spaceRequested);
        printf("External Fragmentation : %.3f\n",1.0 - spaceAllocated/SIZE);
        printf("Space Requested: %ld\n",(long)spaceRequested);
        printf("Space Allocated : %ld\n",(long)spaceAllocated);
        exit(1);
        return 0;
    }
    requests++;
    
    spaceAllocated += blockSize;
    spaceRequested += reqSize;
    struct block * p = available;
    available = available->next;
    char* address = spheap + p->address;
    free(p);
    return address;
}

//Free Memory
void ffree(char * address,size_t reqSize){
    spaceAllocated -= blockSize;
    spaceRequested -= reqSize;
    struct block *p = (struct block *)malloc(sizeof(struct block));
    p->address = address-spheap;
    p->next = available;
    available = p;
}

