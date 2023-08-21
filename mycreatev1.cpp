#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

//main线程堵塞来给线程收尸

void* myprint(void* arg)
{
    printf("%s\n", "hello");
    //return NULL;
    pthread_exit(NULL);         //在线程调用中最好用上专用的线程退出函数，以实现线程的清理
}

int main(void)
{
    pthread_t nnew;
    puts("Begin");
    int ret = pthread_create(&nnew, NULL, myprint, NULL);
    assert(ret == 0);
    ret = pthread_join(nnew, NULL);         //不关心退出的返回值，所以第二个参数为NULL
    assert(ret == 0);
    puts("end");

    return 0;
}