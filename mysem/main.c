#include "mysem.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LEFT 3000000
#define RIGHT 3000201
#define THRNUM (RIGHT - LEFT + 1)

static pthread_t tid[THRNUM];
static mysem_t* my;

static void* func(void *arg)
{
    int num = (int)arg;
    int mark = 1;

    for(int i = 2; i * i <= num; i++)
    {
        if(num % i == 0)
        {
            mark = 0;
            break;
        }
    }
    if(mark)
        fprintf(stdout, "%d is primer\n", num);
    mysem_add(my, 1);
    pthread_exit(NULL);
}

int main(void)
{
    my = mysem_init(5);
    if(my == NULL)
    {
        fprintf(stderr, "initialte failed\n");
        exit(1);
    }

    int i = 0;
    int err;
    for(int i = LEFT; i <= RIGHT; i++)
    {
        err = mysem_sub(my, 1);     
        err = pthread_create(&tid[i - LEFT], NULL, func, (void*)i);
        if(err != 0)
        {
            fprintf(stderr, "pthread_create failed!\n");
            exit(-1);
        }
    }

    for(int i = 0; i < THRNUM; i++)
        pthread_join(tid[i], NULL);

    mysem_destory(my);

    return 0;
}
