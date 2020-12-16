
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include "./multithread.h"
#include <common.h>
#include <utilz.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <unistd.h>
// for C++ only
//#include <iostream> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <string.h>



//https://www.geeksforgeeks.org/ipc-shared-memory/



#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

typedef struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
    int      startWrite;  // offset within shm for start write
    char*    shm;  //shared mem
} tType;

tType myThreads[NUMTHREADS] = {
    {1,1,"1111"}, 
    {2,2,"2222"}, 
    {3,3,"3333"},
    {4,4,"4444"},
    {5,5,"5555"},
};

char shmbuf[MAXBUF];

void* myThreadFun(void*);

// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void* myThreadFun(void *vargp) 
{ 

    sleep(1);
    FILE*f =fopen(MYFILE,"w");
    // get start of shm
    char* threadStr = (char*)((tType*)vargp)->shm;
    fcheck(threadStr == 0, MYTRUE,"shm addr error ",__FILE__,__LINE__);

    printf("Printing from threadId %u    args %s   startWrite %d\tbegin\t%p \n",
        gettid(),((tType*)vargp)->argv_string,((tType*)vargp)->startWrite,threadStr); 

    threadStr += ((tType*)vargp)->startWrite;

    for (int i = 0; i < LOOPCOUNT;i++)
    {
        strcpy(threadStr, ((tType*)vargp)->argv_string);
        threadStr += strlen(((tType*)vargp)->argv_string);
        if (i % 20 == 0)
        {
            //leep(1);
            usleep(1001);
        }

    }
    
    fclose((FILE*) f);
    return NULL;
} 
   
int main() 
{ 

    // check that buf is big enough; all strings must be same len.
    printf("size of shmbuf %d  needed size %ld\n",MAXBUF, LOOPCOUNT*NUMTHREADS*strlen(myThreads[0].argv_string));
    fcheck((MAXBUF <= (LOOPCOUNT*NUMTHREADS*strlen(myThreads[0].argv_string))), MYTRUE," MAXBUF size error ",__FILE__,__LINE__);

    utilzTest();  // check shared lib is out there.

    // start all threads from this process
    for (int i = 0; i <NUMTHREADS; i++)
    {
        myThreads[i].startWrite = i *  LOOPCOUNT * strlen(myThreads[i].argv_string);
        myThreads[i].shm = &shmbuf[0];

        pthread_create(&myThreads[i].thread_id, NULL, myThreadFun, &myThreads[i]); 
        pthread_join(myThreads[i].thread_id, NULL); // join to prev thread
    }
    
    printf("After Joins\n"); 

    printf("\nfinal buffer: %s     len %ld\n",shmbuf,strlen(shmbuf));

    exit(0); 
}
