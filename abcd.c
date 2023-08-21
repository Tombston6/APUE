//该函数展示了4个线程不断的向屏幕分别打印abcd
//通过锁自己解锁另一个线程来实现abcd的打印

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#define THR_NUM 4

static pthread_mutex_t mut[THR_NUM];

static void* func(void* arg)
{
    char c = 'a' + (int)arg;
    int n = (int)arg;
    while (1)
    {
        pthread_mutex_lock(&(mut[n]));              //由于初始化后就被上锁，所以会被阻塞于此
        write(1, &c, 1);
        pthread_mutex_unlock(&(mut[(n+1) % THR_NUM]));
    }
    pthread_exit(NULL);
    
}

int main(void)
{
    pthread_t tid[THR_NUM];
    int err;

    for(int i = 0; i < THR_NUM; i++)
    {
        pthread_mutex_init(&(mut[i]), NULL);
        pthread_mutex_lock(&(mut[i]));                  
        err = pthread_create(&tid[i], NULL, func, (void*)i);
        assert(err == 0);
    }
    pthread_mutex_unlock(&mut[0]);                      //解锁a，这样就能让a不被阻塞，以此来解锁其他的
    alarm(5);
    for(int i = 0; i < THR_NUM; i++)
    {
        pthread_join(tid[i], NULL);
        pthread_mutex_destroy(&(mut[i]));
    }

    return 0;
}