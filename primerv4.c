//使用池类算法完成primer,通知法

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define LEFT 3000000
#define RIGHT 3000200
#define THRNUM 4

static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void* func(void* arg)
{
    int i, j, mark;
    while(1)
    {
        pthread_mutex_lock(&mut);
        
        while(num == 0)
        {
            pthread_cond_wait(&cond, &mut);
            //pthread_mutex_unlock(&mut);
            //sched_yield();
            //pthread_mutex_lock(&mut);
        }
        
        if(num == -1)
        {
            pthread_mutex_unlock(&mut);
            break;
        }
        i = num;
        num = 0;
        pthread_cond_broadcast(&cond);
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
            pthread_cond_wait(&cond, &mut);
        }
        num = i;                
        pthread_cond_signal(&cond);         //下发任务,因为叫醒任何一个都可以，所以可以用pthread_cond_signal
        pthread_mutex_unlock(&mut);
        
    }

    pthread_mutex_lock(&mut);
    while(num != 0)
    {
        pthread_cond_wait(&cond, &mut);
    }
    num = -1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);

    for(int i = 0; i < THRNUM; i++)
        pthread_join(tid[i], NULL);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mut);
    return 0;
}