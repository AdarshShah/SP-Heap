/*
* Name : Adarsh Tushar Shah 
* SRNo : 19473
* Title: Assignment 2
* Date : 20 - 01 - 2022
*/

/*
* The program will terminate only on encountering '=' in the new line.
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

/*
*  
*  Instructions to use oneBin :
*  
*  1. void mem_init(int size) 
*  2. void * mmalloc(size_t reqSize)
*  3. void free(void * ptr,size_t reqSize)
*  
*  In case of Overflow, the system exits with error code 1.
*  
*/
#include "onebin.h"

typedef struct Digit Digit;

struct Digit
{
    // 0 - 999
    int val;

    //Next Higher Order
    Digit *next;
};

//Utility Function
int placevalue(int i){
    switch (i)
    {
    case 1:
        return 1;
    
    case 2:
        return 10;

    case 3:
        return 100;
    }
    return 1;
}

//Display function
void print(Digit* head){
    Digit* stack[1000];
    int top=-1;

    Digit* result=head;
    while(result!=NULL){
        top++;
        stack[top]=result;
        result=result->next;
    }
    if(top>0)
        printf("%d,",stack[top]->val);
    for(int i=top-1;i>0;i--){
        printf("%03d,",stack[i]->val);
    }

    printf("%03d",stack[0]->val);

}

//Generate function
Digit* generate(char *s){
    char *i = s;
    while(*i!='$') {
        i++;
    }
    int c=0;
    int val=0;
    Digit *number = (Digit*)mmalloc(sizeof(Digit)); 
    Digit *head = number;
    while(i>s){
        i--;
        c++;
        if(*i==','){
            number->val = val;
            number->next = (Digit*)mmalloc(sizeof(Digit));
            number = number->next;
            val=0;
            c=0;
        }
        else{
            val = placevalue(c)*(*i-'0') + val;
        }
    }
    if(val>=0){
        number->val = val;
        number->next = NULL;
    }else{
        number->val = -1;
    }

    number = head;
    if(head->next!=NULL){
        while(number->next->next!=NULL){
            number = number->next;
        }
        if(number->next->val==-1){
            ffree(number->next,sizeof(Digit));
            number->next=NULL;
        }
    }
    return head;
}

//Addition Operation
Digit* add(Digit *a,Digit *b){


    Digit* stack[1000];
    int top=-1;

    Digit *result = (Digit*)(mmalloc(sizeof(Digit)));
    Digit *head = result;
    int carry = 0;
    do{
        result->val = carry + (a==NULL?0:a->val) + (b==NULL?0:b->val);
        carry = result->val/1000;
        result->val%=1000;
        if(a!=NULL) a = a->next;
        if(b!=NULL) b = b->next;
        result->next = generate("0$");
        result = result->next;
    }while(a!=NULL || b!=NULL || carry);
    result = head;
    while(result->next->next!=NULL)
        result=result->next;
    ffree(result->next,sizeof(Digit));
    result->next=NULL;

    result=head;
    while(result!=NULL){
        top++;
        stack[top]=result;
        result=result->next;
    }

    while(top>0 && stack[top]->val==0){
       stack[top-1]->next=NULL;
       ffree(stack[top],sizeof(Digit));
        top--;
    }

    return head;
}

//Shift-Multiply Operation
Digit* shiftMultiply(Digit *a,int n,int b){
    Digit* head = generate("0$");
    Digit* curr1 = head;
    for(int i=0 ; i<n/3 ; i++){
        curr1->next = generate("0$");
        curr1 = curr1->next;
    }
    Digit* thead = generate("0$");
    Digit* curr = thead;
    int c = placevalue(n%3+1);
    int carry = 0;
    
    while(a!=NULL){
        curr->val = a->val*c*b + carry;
        carry= curr->val/1000;
        curr->val%=1000;
        if(a->next!=NULL || carry)
            curr->next = generate("0$");
        if(a->next==NULL && carry){
            curr->next->val = carry;
            curr->next->next = NULL;
            break;
        }
        a = a->next;
        curr = curr->next;
    }
    curr1->next = thead;
    return head->next;
}

//Multiplication Operation
Digit* multiply(Digit *a,Digit *b){
    Digit* stack[1000];
    int top=-1;

    Digit* head = generate("0$");
    int carry=0;
    int sh=0;
    int c=0;
    Digit* curr;
    while(b!=NULL){
        Digit* curr = shiftMultiply(a,3*sh+c,b->val%10);
        head = add(head,curr);
        b->val = b->val/10;
        c++;
        if(b->val==0){
            c=0;
            sh++;
            b=b->next;
        }
    }   

    Digit* result=head;
    while(result!=NULL){
        top++;
        stack[top]=result;
        result=result->next;
    }

    while(top>0 && stack[top]->val==0){
       stack[top-1]->next=NULL;
       ffree(stack[top],sizeof(Digit));
        top--;
    }

    return head;
}

char str[10000];
char num[10000];
char operator;

void testcases(){
    int terms = rand()/(RAND_MAX+1.0)*10+2;
    int k=0;
    while(terms--){
        int digits = rand()/(RAND_MAX+1.0)*10+1;
        while(digits--){
            str[k++]='0'+rand()%10;
            if(digits%3==0)
                str[k++]=',';
        }
        k--;
        str[k++]='$';
        if(rand()&1)
            str[k++]='+';
        else
            str[k++]='*';
    }
    k--;
    str[k++]='=';
    str[k++]='\0';
}

int main(){
    mem_init(sizeof(struct block));
    int take_inp=0;
    clock_t start = clock();
    testcases();
    double avg_clock = 0;
    long n=1;
    
    if(!(str[0]>='0' && str[0]<='9')){
        return 0;
    }
    
    int i=0;
    while(str[i]!='$') {
        num[i]=str[i];
        i++;
    }
    num[i]='$';
    i++;

    if(str[i]=='\0')
        take_inp=1;

    Digit* top = generate(num);

    while(1){

        if(take_inp){
            scanf("%s",str);
            i=0;
        }
        operator=str[i];
        i++;    

        Digit *operand;
        if(operator!='='){
            if(str[i]=='\0'){
                take_inp=1;
                i=0;
            }else{
                take_inp=0;
            }
            

            if(take_inp){
                scanf("%s",str);
                i=0;
            }

            int j=0;
            while(str[i]!='$') {
                num[j]=str[i];
                i++;
                j++;
            }
            num[j]='$';
            i++;
            if(str[i]=='\0')
                take_inp=1;
            else
                take_inp=0;


            operand = generate(num);
        }

        switch (operator)
        {
        case '+':
            top = add(top,operand);
            break;
        
        case '*':
            top = multiply(top,operand);
            break;
        
        default:
            printf("\nINVALID INPUT");
            exit(1);
        
        case '=':
            avg_clock = avg_clock + (clock()-start-avg_clock)/(double)(n++);
            if(n%50==0){
                printf("\n\nInternal Fragmentation : %.3f \n",(spaceAllocated-spaceRequested)/spaceRequested);
                printf("External Fragmentation : %.3f\n",1.0 - spaceAllocated/SIZE);
                printf("Space Requested: %ld\n",(long)spaceRequested);
                printf("Space Allocated : %ld\n",(long)spaceAllocated);
                printf("Avg Clock Cycles : %.2f\n",avg_clock);
                printf("Total Equations solved : %d\n\n",n);
            }
            start = clock();
            take_inp=0;
            i=0;
            testcases();
            if(!(str[0]>='0' && str[0]<='9')){
                return 0;
            }
            while(str[i]!='$') {
                num[i]=str[i];
                i++;
            }
            num[i]='$';
            i++;

            if(str[i]=='\0')
                take_inp=1;
            else
                take_inp=0;

            top = generate(num);
            break;
        }
    }
    return 0;
}