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
#include "spheap.h"
#include <math.h>


int uniform(int a,int b){
    return rand()/(RAND_MAX+1.0) * (b-a) + a;
}

int exponential(double lambda){
    double u = rand() / (RAND_MAX + 1.0);
    int x = -1*log(1- u) * lambda;
    if(x<100)
        x=100;
    else if(x>2000)
        x=2000;
    return x;
}

struct node{
    char  *address;
    int size;
    struct node* next;
};

struct node* make_node(char *address,int size){
    struct node * p = (struct node *)malloc(sizeof(struct node));
    p->address = address;
    p->size = size;
    p->next = NULL;
    return p;
}

void empty(struct node* q,int flag){
    if(q==NULL)
        return;
    if(flag)
        ffree(q->address,q->size);
    empty(q->next,flag);
    free(q);
}

struct node *Q[100]={NULL};

int main(){
    
    printf("To perform simulation, Enter U for Uniform request generation and E for Exponential request generation : ");
    char c = getchar();
    switch (c)
    {
    case 'U':
        for( int Time=0;  ;Time++){
            if(Time%200 == 0 && Time!=0){
                printf("\n\nInternal Fragmentation : %.3f \n",(spaceAllocated-spaceRequested)/spaceRequested);
                printf("External Fragmentation : %.3f\n",1.0 - spaceAllocated/SIZE);
                printf("Total Splits : %lld\n",splits);
                printf("Total Combines : %lld\n",combines);
                printf("Total Requests : %lld\n\n",requests);
            }
            if(Time<2000)
                empty(Q[Time%100],1);
            else
                empty(Q[Time%100],0);
            Q[Time%100]=NULL;
            int size = uniform(100,2000);
            char *p = mmalloc(size);
            int life = uniform(1,100);
            struct node *n = make_node(p,size);
            n->next = Q[(Time+life)%100];
            Q[(Time+life)%100] = n;
        }
        break;
    
    case 'E':
        for( int Time=0;  ;Time++){
            if(Time%200 == 0 && Time!=0){
                printf("\n\nInternal Fragmentation : %.3f \n",(spaceAllocated-spaceRequested)/spaceRequested);
                printf("External Fragmentation : %.3f\n",1.0 - spaceAllocated/SIZE);
                printf("Total Splits : %lld\n",splits);
                printf("Total Combines : %lld\n",combines);
                printf("Total Requests : %lld\n\n",requests);
            }
            if(Time<2000)
                empty(Q[Time%100],1);
            else
                empty(Q[Time%100],0);
            Q[Time%100]=NULL;
            int size = exponential(1000);
            char *p = mmalloc(size);
            int life = uniform(1,100);
            struct node *n = make_node(p,size);
            n->next = Q[(Time+life)%100];
            Q[(Time+life)%100] = n;
        }
        break;
    }
    
    return 0;
}