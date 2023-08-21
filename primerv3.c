//使用池类算法完成primer,盲等版 不太好

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define LEFT 3000000
#define RIGHT 3000200
#define THRNUM 4

static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void* func(void* arg)
{
    int i, j, mark;
    while(1)
    {
        pthread_mutex_lock(&mut);
        while(num == 0)
        {
            pthread_mutex_unlock(&mut);
            sched_yield();
            pthread_mutex_lock(&mut);
        }
        if(num == -1)
        {
            pthread_mutex_unlock(&mut);
            break;
        }
        i = num;
        num = 0;
        pthread_mutex_unlock(&mut);

        mark = 1;
        for(j = 2; j * j <= i; j++)
        {
            if(i % j == 0)
            {
                mark = 0;
                break;
            }
        }
        if(mark)
            printf("[%d]%d is a primer\n", (int)arg,i);
    }
    
    pthread_exit(NULL);
}

int main(void)
{
    int err;
    pthread_t tid[THRNUM];

    for(int i = 0; i < THRNUM; i++)
    {
        err = pthread_create(&tid[i], NULL, func, (void*)i);
        assert(err == 0);
    }

    for(int i = LEFT; i <= RIGHT; i++)
    {
        pthread_mutex_lock(&mut);       //先锁住互斥量

        while(num != 0)
        {
            //这里使用while是因为，当num是0则表示现在暂时没有任务
            //那么就需要把锁打开，让大家一起来抢，抢到再锁住
            pthread_mutex_unlock(&mut);
            sched_yield();          //出让调度器给其他线程，可以理解为一个很小的sleep，不加这个多半情况还是会被自己拿到锁
            pthread_mutex_lock(&mut);
        }
        num = i;                //下发任务
        pthread_mutex_unlock(&mut);
        
    }

    pthread_mutex_lock(&mut);
    while(num != 0)
    {
        pthread_mutex_unlock(&mut);
        sched_yield();          //出让调度器给其他线程，可以理解为一个很小的sleep，不加这个多半情况还是会被自己拿到锁
        pthread_mutex_lock(&mut);
    }
    num = -1;
    pthread_mutex_unlock(&mut);

    for(int i = 0; i < THRNUM; i++)
        pthread_join(tid[i], NULL);
    pthread_mutex_destroy(&mut);
    return 0;
}