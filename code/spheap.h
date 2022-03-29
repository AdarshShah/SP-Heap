#include <stdlib.h>
#include <stdio.h>

/*
*  
*  Instructions to use spheap :
*  
*  1. void * mmalloc(size_t reqSize)
*  2. void free(void * ptr,size_t reqSize)
*  
*  In case of Overflow, the system exits with error code 1.
*  
*/

// Heap Size : 256 MB = 268435456 Bytes
#define SIZE 268435456

// Static Allocation : starting address 
char spheap[SIZE];

//Track Internal Fragmentation
double spaceAllocated = 0;
double spaceRequested = 0;

//track splits & combines
long long int splits=0;
long long combines=0;

//track number of requests
long long requests=0;

int recordSpaceAllocation(int size,int operation){
    if (operation)
    {
        requests++;
        spaceRequested += size;
        if(size==1){
            spaceAllocated += 1;
            return 1;
        }
        else if(size==2){
            spaceAllocated += 2;
            return 2;
        }
        else{
            int idx[56];
            idx[0] = 1;
            idx[1] = 2;
            for(int i=2 ; i<56 ; i++){
                if(i&1)
                    idx[i] = idx[i-2]<<1;
                else
                    idx[i] = 3*(idx[i-1]>>1);
                if(size<=idx[i]){
                    spaceAllocated += idx[i];
                    return idx[i];
                }
            }
        }
    }else{
        spaceRequested -= size;
        if(size==1){
            spaceAllocated -= 1;
            return 1;
        }
        else if(size==2){
            spaceAllocated -= 2;
            return 2;
        }
        else{
            int idx[56];
            idx[0] = 1;
            idx[1] = 2;
            for(int i=2 ; i<56 ; i++){
                if(i&1)
                    idx[i] = idx[i-2]<<1;
                else
                    idx[i] = 3*(idx[i-1]>>1);
                if(size<=idx[i]){
                    spaceAllocated -= idx[i];
                    return idx[i];
                }
            }
        }
    }
    return -1;
}

//Intialization Flag
int flag = 0;

struct block{
    int address;
    int TAG;
    int TYPE;
    int KVAL;
    struct block *parent;
    struct block* prev;
    struct block* next;
};

//generator
struct block* make_block(int address,int type,int kval,struct block *parent){
    struct block* p = (struct block*)malloc(sizeof(struct block));
    p->address=address;
    p->TAG=1;
    p->TYPE=type; 
    p->KVAL=kval; // size
    p->parent = parent;
    p->next=p->prev=NULL;
}

//Available Space List
struct block* available[56] = {NULL};

//to check initialization
int is_init=0;

int _index(int size){
    if(size==1)
        return 0;
    if(size==2)
        return 1;
    int idx[56];
    idx[0] = 1;
    idx[1] = 2;
    for(int i=2 ; i<56 ; i++){
        if(i&1)
            idx[i] = idx[i-2]<<1;
        else
            idx[i] = 3*(idx[i-1]>>1);
        if(size<=idx[i])
            return i;
    }
    return -1;
}

//To view heap allocation. For testing only.
void util_print_heap(){
    int idx[56];
    idx[0] = 1;
    idx[1] = 2;
    for(int i=2 ; i<56 ; i++){
        if(i&1)
            idx[i] = idx[i-2]<<1;
        else
            idx[i] = 3*(idx[i-1]/2);
    }

    printf("\n\n");
    for(int i=0 ; i<40 ; i++)
        printf("#");
    printf("\n");
    for(int i=0;i<56;i++){
        struct block *p = available[i];
        printf("\nBLOCK SIZE : %d\n",idx[i]);
        while (p!=NULL){
            printf("%d\t%d\t%d\n",p->address,p->TAG,p->KVAL);
            p=p->next;
        }
    }
    for(int i=0 ; i<40 ; i++)
        printf("#");
    printf("\n\n");
}

//Insert at a particular location in Available space list
void insert_block(struct block *p,int idx){
    p->next = available[idx];
    if(available[idx]!=NULL)
        available[idx]->prev = p;
    available[idx] = p;
}

//Remove at a particular location in Available space list
void remove_block(struct block *p,int idx){
    if(available[idx]==p){
        available[idx] = p->next;
        if(available[idx]!=NULL)
            available[idx]->prev=NULL;
    }else{
        p->prev->next = p->next;
        if(p->next!=NULL)
            p->next->prev = p->prev;
    }
    free(p);
}

//Perform split operation iteratively
struct block* allocate(struct block* p,int reqSize ){   
    if(((p->address>>p->KVAL)&2)==2 && p->KVAL==1){
        printf("DUCK\n");
    }
    splits++;
    //printf("%d\t%d\t%d\t%d\n",p->address,p->KVAL,p->TYPE,reqSize);
    p->TAG=0;
    int size = 1<<p->KVAL;
    struct block *h1;
    struct block *h2;
    switch(p->TYPE){
        case 1:
        case 2:
            //printf("12::%d\t%d\t%d\t%d\n",p->address,p->TYPE,p->KVAL,size);
            h1 = make_block(p->address,3,p->KVAL-2,p);
            h2 = make_block(p->address+3*(1<<p->KVAL-2),3,p->KVAL-2,p);
            p->TAG=0;
            if(reqSize>3*(1<<h1->KVAL) && reqSize>(1<<h2->KVAL))
                return p;
            insert_block(h1,_index(3*(1<<h1->KVAL)));
            insert_block(h2,_index(1<<h2->KVAL));
            if(3*(1<<h1->KVAL)==reqSize){
                return h1;
            }
            else if((1<<h2->KVAL)==reqSize){
                return h2;
            }
            else if(3*(1<<h1->KVAL)>reqSize)
                return allocate(h1,reqSize);
            else
                return allocate(h2,reqSize);
        case 3:
            if(p->address==p->parent->address){
                size = 3*size;
                //printf("30::%d\t%d\t%d\t%d\n",p->address,p->TYPE,p->KVAL,size);
                h1 = make_block(p->address,1,p->KVAL+1,p);
                h2 = make_block(p->address+(1<<h1->KVAL),2,p->KVAL,p);

                if(reqSize>(1<<h1->KVAL) && reqSize>(1<<h2->KVAL))
                return p;

                insert_block(h1,_index(1<<h1->KVAL));
                insert_block(h2,_index(1<<h2->KVAL));
                if((1<<h1->KVAL)==reqSize)
                    return h1;
                else if((1<<h2->KVAL)==reqSize)
                    return h2;
                else if((1<<h1->KVAL)>reqSize)
                    return allocate(h1,reqSize);
                else if((1<<h2->KVAL)>reqSize)
                    return allocate(h2,reqSize);
                else
                    return p;    
            }else{
                //printf("31::%d\t%d\t%d\t%d\n",p->address,p->TYPE,p->KVAL,size);
                h1 = make_block(p->address,3,p->KVAL-2,p);
                h2 = make_block(p->address+3*(1<<p->KVAL-2),3,p->KVAL-2,p);
                p->TAG=0;
                if(reqSize>3*(1<<h1->KVAL) && reqSize>(1<<h2->KVAL))
                    return p;
                insert_block(h1,_index(3*(1<<h1->KVAL)));
                insert_block(h2,_index(1<<h2->KVAL));
                if(3*(1<<h1->KVAL)==reqSize){
                    return h1;
                }
                else if((1<<h2->KVAL)==reqSize){
                    return h2;
                }
                else if(3*(1<<h1->KVAL)>reqSize)
                    return allocate(h1,reqSize);
                else if((1<<h2->KVAL)>reqSize)
                    return allocate(h2,reqSize);
                else
                    return p;    
            }
    }
}

//Custom Malloc
void * mmalloc(int size){
    //Track Internal Fragmentation Metrics
    //Initialization
    if(!is_init){
        available[55] = make_block(0,1,28,NULL);
        is_init=1;
    }
    for(int i=_index(size) ; i<56 ; i++){
        struct block *p = available[i];
        while (p!=NULL && !p->TAG)
            p = p->next;
        if(p!=NULL){
            int reqSize = recordSpaceAllocation(size,1);
            
            //printf("%d\t%d\t%d\t%d\n",size,reqSize,p->address,p->TYPE);
            p = allocate(p,reqSize);
            p->TAG=0;
            //printf("%d\t%d\t%d\n",size,reqSize,p->address);
            return spheap+p->address;
        }
    }
    printf("\n");
    for(int i=0 ; i<40 ; i++)
        printf("#");

    printf("\n\nRequested Size: %d \n",size);
    printf("\n\nInternal Fragmentation : %.3f \n",(spaceAllocated-spaceRequested)/spaceRequested);
    printf("External Fragmentation : %.3f\n",1.0 - spaceAllocated/SIZE);
    printf("Total Splits : %lld\n",splits);
    printf("Total Combines : %lld\n",combines);
    printf("Total Requests : %lld\n\n",requests);

    //util_print_heap();
    exit(1);
    return NULL;
}

// //Combine Buddies iteratively
// void combine(struct block *p,int idx){
//     int size[56];
//     size[0] = 1;
//     size[1] = 2;
//     for(int i=2 ; i<56 ; i++){
//         if(i&1)
//             size[i] = size[i-2]<<1;
//         else
//             size[i] = 3*(size[i-1]/2);
//     }
//     combines++;
//     int address;
//     int idx2;
//     struct block * h;
//     switch(p->TYPE){
//         case 1:
//             address = p->address + (1<<p->KVAL);
//             idx2 = _index(1<<p->KVAL-1); 
//             break;
//         case 2:
//             address = p->address - (1<<p->KVAL+1);
//             idx2 = _index(1<<p->KVAL+1); 
//             break;
//         case 3:
//             switch (size[idx]%3)
//             {
//             case 0:
//                 address = p->address + 3*(1<<p->KVAL);
//                 idx2 = _index(1<<p->KVAL); 
//                 break;
//             default:
//                 address = p->address - 3*(1<<p->KVAL);
//                 idx2 = _index(3*(1<<p->KVAL)); 
//                 break;
//             }
//     }
//     for(h=available[idx2];h!=NULL && h->address!=address;h=h->next);
//     if(h==NULL || h->TAG==0){
//         return;
//     }else{
//         //combine
//         int new_address = h->address<p->address?h->address:p->address;
//         int TYPE;
//         int KVAL;
//         int idx3;
//         if(h->KVAL==p->KVAL){
//             KVAL = h->KVAL+2;
//             idx3 = _index(1<<KVAL);
//             if(((new_address>>KVAL)&3)==3){
//                 TYPE=3;
//             }else if(((new_address>>KVAL)&2)==2){
//                 TYPE=2;
//             }else{
//                 TYPE=1;
//             }
            
//             remove_block(p,idx);
//             remove_block(h,idx2);
//             struct block * q = make_block(new_address,TYPE,KVAL);
//             insert_block(q,idx3);
//             combine(q,idx3);
            
//         }else{
//             KVAL = p->KVAL<h->KVAL?p->KVAL:h->KVAL;
//             idx3 = _index(3*(1<<KVAL));
            
//             remove_block(p,idx);
//             remove_block(h,idx2);
//             struct block * q = make_block(new_address,TYPE,KVAL);
//             insert_block(q,idx3);
//             combine(q,idx3);
//         }
//     }
// }

//combine buddies
void combine(struct block* p,int idx){
    combines++;
    if(p==NULL){
        return;
    }
    p->TAG=1;
    if(p->parent==NULL){
        p->TAG=1;
        return;
    }
    struct block * buddy;
    struct block * parent = p->parent;
    int idx2;
    int idx3;
    if(p->parent->address==p->address){
        if(p->TYPE==3){
            idx2 = _index(1<<p->KVAL);
            idx3 = _index(1<<p->KVAL+2);
            buddy = available[idx2];
        }
        else if(p->TYPE==1){
            idx2 = _index(1<<p->KVAL-1);
            idx3 = _index(3*(1<<p->KVAL-1));
            buddy = available[idx2];
        }else{
            printf("DUCK!! Left");
            exit(1);
        }
    }else{
        if(p->TYPE==3){
            idx2 = _index(3*(1<<p->KVAL));
            idx3 = _index(1<<p->KVAL+2);
            buddy = available[idx2];
        }else if(p->TYPE==2){
            idx2 = _index(1<<p->KVAL+1);
            idx3 = _index(3*(1<<p->KVAL));
            buddy = available[idx2];
        }else{
            printf("DUCK!! Right");
            exit(1);
        }
    }
    while(buddy!=NULL && buddy->parent!=p->parent)
        buddy=buddy->next;
    if(buddy!=NULL && buddy->TAG==1){
        remove_block(p,idx);
        remove_block(buddy,idx2);
        combine(parent,idx3);
    }
}

//freeing blocks
void ffree(char * address,int size){
    //Track Internal Fragmentation Metrics
    recordSpaceAllocation(size,0);
    int addr = address - spheap;
    int idx = _index(size);
    struct block* p = available[idx];
    while(p!=NULL && p->address!=addr){
        p = p->next;
    }
    if(p==NULL){
        printf("Not Found");
    }else{
        combine(p,idx);
    }
}