//创建线程并打印pid和主tid和新创建tid的线程号
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

pthread_t ntid;

void printids(const char* s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();           //调用函数自己获取线程号，而不是从共享内存中读出
    printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

//线程任务
void* thr_fn(void *arg)
{
    printids("new thread: ");
    return ((void*)0);
}

int main(void)
{
    int err;

    err = pthread_create(&ntid, NULL, thr_fn, NULL);    //创建线程
    if(err != 0)
        perror("phread wrong");
    printids("main thread:");
    sleep(1);                                           //如果主线程不休眠，它就可能立即退出，这样新创建的线程没来得及运行

    return 0;
}