#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define LEFT 3000000
#define RIGHT 3000200
#define THRNUM (RIGHT - LEFT + 1)

//避免竞争，使用池类算法
struct thr_arg_st
{
    int n;
};

static void* func(void* arg)
{
    int tmp = ((struct thr_arg_st*)arg)->n;
    for(int i = 2; i * i <= tmp; i++)
    {
        if(tmp % i == 0)
        {
            printf("%d\n", tmp);
            break;
        }
    }
    pthread_exit(arg);
}

int main(void)
{
    int i, j , mark;
    pthread_t tid[THRNUM];
    int err;
    struct thr_arg_st* p;
    void *ptr;

    for(i = 0 ; i < THRNUM; i++)
    {
        p = (struct thr_arg_st*)malloc(sizeof(struct thr_arg_st));
        assert(p != NULL);
        p->n = i;
        err = pthread_create(&tid[i], NULL, func, (void *)p);
        assert(err == 0);
    }

    for(i = 0; i < THRNUM; i++ )
    {
        pthread_join(tid[i], &ptr);         //收尸并free,等待线程结束并接受线程结束返回的值
        free(ptr);
    }

    return 0;
}