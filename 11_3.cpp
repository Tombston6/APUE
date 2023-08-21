//验证pthread_join()函数等待指定的线程终止
#include <stdio.h>
#include <pthread.h>

void *thr_fn1(void *arg)
{
    printf("thread 1 returning\n");
    return((void*)1);
}

void *thr_fn2(void* arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void*)2);
}

int main(void)
{
    int err;
    pthread_t tid1, tid2;
    void *tret;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if(err != 0)
        perror("pthread_create wrong");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if(err != 0)
        perror("pthread_creat wrong");
    
    //从启动例程中退出
    err = pthread_join(tid1, &tret);
    if(err != 0)
        perror("pthread_join wrong");
    printf("thread 1 exit code %ld\n", (long)tret);
    
    //使用pthread_exit()退出线程
    err = pthread_join(tid2, &tret);
    if(err != 0)
        perror("phread_join wrong");
    printf("thread 2 exit code %ld\n", (long)tret);

    return 0;
}