#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

static void clean_up_func(void* p)
{
    puts((char*)p);
}

static void* func(void* arg)
{
    puts("hello");

    //注意成对出现
    pthread_cleanup_push(clean_up_func, (void*)"cleanup1");
    pthread_cleanup_push(clean_up_func, (void*)"cleanup2");
    pthread_cleanup_push(clean_up_func, (void*)"cleanup3");
    puts("push over");
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
    
    
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;

    puts("Begin!");
    int err = pthread_create(&tid, NULL, func, NULL);
    assert(err == 0);

    err = pthread_join(tid, NULL);
    puts("END");
    return 0;
}