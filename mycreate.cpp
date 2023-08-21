#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

//新创建的线程还没有被调度，这个进程就结束了，所以不能打印出hello

void* myprint(void* arg)
{
    printf("%s\n", "hello");
    return NULL;
}

int main(void)
{
    pthread_t nnew;
    puts("Begin");
    int ret = pthread_create(&nnew, NULL, myprint, NULL);
    assert(ret == 0);
    puts("end");

    return 0;
}