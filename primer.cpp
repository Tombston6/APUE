#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define LEFT 3000000
#define RIGHT 3000200
#define THRNUM (RIGHT - LEFT + 1)

//使用多线程来并行处理primer,出现了竞争现象，导致出错
//这是因为传递的是一个地址

static void* func(void* arg)
{
    int tmp = *(int*)arg;
    for(int i = 2; i * i <= tmp; i++)
    {
        if(tmp % i == 0)
        {
            printf("%d\n", tmp);
            break;
        }
    }
    pthread_exit(NULL);
}

int main(void)
{
    int i, j , mark;
    pthread_t tid[THRNUM];
    int err;

    for(i = 0 ; i < THRNUM; i++)
    {
        err = pthread_create(&tid[i], NULL, func, (void *)&i);
        assert(err == 0);
    }

    for(i = 0; i < THRNUM; i++ )
        pthread_join(tid[i], NULL);

    return 0;
}