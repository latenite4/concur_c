

// C program to use shared memory by multiple threads and semaphores.
//name: R. Melton
//date: 12/16/20
//
//clone:  git clone https://github.com/latenite4/concur_c.git and git clone https://github.com/latenite4/utilz_c.git
//compile: make clean; make all
//run:   ./concur  OR ./concur sem (to use semaphores in child threads.)

//note: usually there is no colision (race condition) between threads,
//with or without semaphores.  the shared buffer is dumped at the end of the program.


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
#include <semaphore.h> 


//https://www.geeksforgeeks.org/ipc-shared-memory/



#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))
sem_t sem;  // semaphore for threads

typedef struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       usesem;            // should threads use semaphores?
    char     *argv_string;      /* From command-line argument */
    int      startWrite;  // offset within shm for start write
    char*    shm;  //shared mem
    sem_t*   sem;

} tType;

tType myThreads[NUMTHREADS] = {
    {1,0,"1111",0,NULL}, 
    {2,0,"2222",0,NULL}, 
    {3,0,"3333",0,NULL},
    {4,0,"4444",0,NULL},
    {5,0,"5555",0,NULL}
};

char shmbuf[MAXBUF];

void* myThreadFun(void*);

// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void* myThreadFun(void *vargp) 
{ 

    // get start of shm
    char* threadStr = (char*)((tType*)vargp)->shm;
    fcheck(threadStr == 0, MYTRUE,"shm addr error ",__FILE__,__LINE__);

    printf("Printing from threadId %u    args %s   startWrite %d\tbegin\t%p sem %p\n",
        gettid(),((tType*)vargp)->argv_string,((tType*)vargp)->startWrite,threadStr,((sem_t*)((tType*)vargp)->sem )); 

    threadStr += ((tType*)vargp)->startWrite;

    sem_t* sem= ((sem_t*)((tType*)vargp)->sem );

    printf("sem %p",sem);

    if( ((int)((tType*)vargp)->usesem) == MYTRUE ) 
    {
        sem_wait(sem); 
    }

    // do all writes for this thread.
    for (int i = 0; i < LOOPCOUNT;i++)
    {
        strcpy(threadStr, ((tType*)vargp)->argv_string);
        threadStr += strlen(((tType*)vargp)->argv_string);
        if (i % 20 == 0)
        {
            usleep(2002); //useconds
        }

    }
    
    if( ((int)((tType*)vargp)->usesem) == MYTRUE ) 
    {
        sem_post(sem); 
    }

    pthread_exit(NULL);
} 
   
int main( int argc, char *argv[] )
{ 
    printf("\n\nnum threads: %d num writes per thread: %d total data written to shared memory by all threads: %ld bytes.\n\n",NUMTHREADS,LOOPCOUNT,
        LOOPCOUNT*NUMTHREADS*strlen(myThreads[0].argv_string));

    int usesem = MYFALSE;
    // check for CLI flag telling whether to use semaphores
    if (argc == 2 && strcmp(argv[1],"sem") == 0)
    {
        usesem = MYTRUE;
       
    }
    int ret = sem_init(&sem, 1, 1);
    fcheck(ret  != NOERROR, MYTRUE," sem_init error ",__FILE__,__LINE__);

    printf("sem main %p",&sem);


    // check that buf is big enough; all strings must be same len.
    printf(" size of shmbuf %d  needed size %ld\n",MAXBUF, LOOPCOUNT*NUMTHREADS*strlen(myThreads[0].argv_string));
    fcheck((MAXBUF <= (LOOPCOUNT*NUMTHREADS*strlen(myThreads[0].argv_string))), MYTRUE," MAXBUF size error ",__FILE__,__LINE__);

    utilzTest();  // check shared lib is out there.

    // start all threads from this process
    for (int i = 0; i <NUMTHREADS; i++)
    {
        myThreads[i].startWrite = i *  LOOPCOUNT * strlen(myThreads[i].argv_string);
        myThreads[i].shm = &shmbuf[0];
        myThreads[i].usesem = usesem;
        myThreads[i].sem = &sem;
        pthread_create(&myThreads[i].thread_id, NULL, myThreadFun, &myThreads[i]); 
        pthread_join(myThreads[i].thread_id, NULL); // join to prev thread
    }
    
    printf("After Joins\n"); 

    char* semused = " ";
    if(usesem == MYTRUE)
    {
        semused = "semaphores used by threads.";
        sem_destroy(&sem); 
    }
    printf("\nfinal buffer: %s     len %ld %s\n",shmbuf,strlen(shmbuf),semused);

    
    exit(0); 
}
