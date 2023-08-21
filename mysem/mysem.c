#include <pthread.h>
#include <stdio.h>
#include "mysem.h"
#include <stdlib.h>
#include <errno.h>

struct mysem_st
{
    int value;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

mysem_t* mysem_init(int initval)
{
    struct mysem_st *me;
    me = (struct mysem_st *)malloc(sizeof(struct mysem_st));
    if(me == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return NULL;
    }

    me->value = initval;
    pthread_mutex_init(&(me->mut), NULL);
    pthread_cond_init(&(me->cond), NULL);

    return me;
}

int mysem_add(mysem_t* ptr, int val)
{
    if(val <= 0)
        return -EINVAL;

    struct mysem_st* me = ptr;

    pthread_mutex_lock(&me->mut);
    me->value += val;
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut);
    
    return val;
}

int mysem_sub(mysem_t *ptr, int val)
{   
    if(val <= 0)
        return -EINVAL;

    struct mysem_st* me = ptr;
    pthread_mutex_lock(&me->mut);
    while(me->value < val)
        pthread_cond_wait(&me->cond, &me->mut);
    me->value -= val;
    pthread_mutex_unlock(&me->mut);
    return val;
}

int mysem_destory(mysem_t* ptr)
{
    struct mysem_st *me = ptr;

    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(me);

    return 0;
}