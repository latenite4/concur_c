
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include "./multithread.h"
#include <common.h>
#include <utilz.h>
#include <unistd.h>
#include <sys/syscall.h>

#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

typedef struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
} tType;

tType myThreads[NUMTHREADS] = {{1,1,"1111"}, {2,2,"2222"}, {3,3,"3333"}};


// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void* myThreadFun(void *vargp) 
{ 

    FILE *f = fopen(MYFILE,"a");
    pthread_t tid = gettid();
    fcheck(f == NULL, MYTRUE, "file open ");
    printf("Printing from threadId %lu    args %s\n",tid,(char*)vargp); 
    static int j = 0;
    for (int i = 0; i < LOOPCOUNT;i++)
    {
        fprintf(f,"%s",(char*)vargp);

    }
    myThreads[j++].thread_id = tid;

    int err = fclose(f);
    fcheck(err != NOERROR, MYTRUE, "file close ");
    return NULL;
} 
   
int main() 
{ 

    utilzTest();

    //pthread_t thread_id; 
    printf("Before Threads\n"); 
   

    remove(MYFILE);

    for (int i = 0; i <NUMTHREADS; i++)
    {
        pthread_create(&myThreads[i].thread_id, NULL, myThreadFun, myThreads[i].argv_string); 
        pthread_join(myThreads[i].thread_id, NULL); 
    }
    
    printf("After Joins\n"); 
    exit(0); 
}
