#include "mytbf.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

static struct mytbf_st* job[MYTBF_MAX];
static pthread_mutex_t mutjob = PTHREAD_MUTEX_INITIALIZER;
static int inited = 0;
static tid;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut;        //每个房间自己的锁
    pthread_cond_t cond;        //用于通知法
};

static int get_free_pos_unlock(void)
{
    int i;
    for(i = 0; i < MYTBF_MAX; i++)
    {
        //这里也是在查看job数组，所以按理来说也应该加锁，但没必要，所以需要提醒
        if(job[i] == NULL)          
            return i;
    }
    return -1;
}

static void* func(void* arg)
{
    int i;

    while(1)
    {
        pthread_mutex_lock(&mutjob);
        for(i = 0; i < MYTBF_MAX; i++)
        {
            if(job[i] != NULL)
            {
                pthread_mutex_lock(&(job[i]->mut));
                job[i]->token += job[i]->cps;
                if(job[i]->token > job[i]->burst)
                    job[i]->token = job[i]->burst;
                pthread_cond_broadcast(&job[i]->cond);  //广播发送
                pthread_mutex_unlock(&(job[i]->mut));
            }
        }
        pthread_mutex_unlock(&mutjob);
        sleep(1);           //应该用一个安全的替换掉sleep函数
    }
}

static void module_unload(void)
{
    //此模块应该只调用一次，如果调用多次有可能会出现错误
    int i;

    pthread_cancel(tid);
    pthread_join(tid, NULL);

    for(i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL)
        {
            mytbf_destory(&job[i]);
        }
    }
    pthread_mutex_destroy(&mutjob);
}

static void module_load(void)
{
    int err;

    err = pthread_create(&tid, NULL, func, NULL);
    assert(err == 0);

    atexit(module_unload);
}

mytbf_t* mytbf_init(int cps, int burst)
{
    struct mytbf_st* me;
    int pos;

    /*
    lock()
    //如果没有初始化，就初始化
    if(!inited)
    {
        module_load();
        inited = 1;
    }
    unlock()
    */
   pthread_once(&init_once, module_load);
    
    me = malloc(sizeof(struct mytbf_st));
    if(me == NULL)
        return NULL;
    
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);

    pthread_mutex_lock(&mutjob);            //在这上锁是因为避免多个函数同时取得一个位置，要减少临界区代码的长度
    pos = get_free_pos_unlock();                   //这也是一个跳转语句    
    if(pos < 0)
    {
        pthread_mutex_unlock(&mutjob);
        free(me);
        return NULL;
    }
    me->pos = pos;

    job[pos] = me;
    pthread_mutex_unlock(&mutjob);

    return me;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int mytbf_fetchtoken(mytbf_t* ptr, int size)
{
    struct mytbf_st *me = ptr;
    int n;

    if(size <= 0)
        return -EINVAL;
    
    pthread_mutex_lock(&me->mut);

    
    while(me->token <= 0)
    {
        /*
        //此部分是一个盲等状态，会导致负载，查询法
        pthread_mutex_unlock(&me->mut);
        sched_yield();
        pthread_mutex_lock(&me->mut);
        */

       //发现没有token值就解锁等待，等待到之后马上开始抢锁
       pthread_cond_wait(&me->cond, &me->mut);
    }


    n = min(me->token, size);           //获取更小的
    me->token -= n;                     //此处在修改，所以要注意是否发生竞争
    pthread_mutex_unlock(&me->mut);                     
    return n;
}

int mytbf_returntoken(mytbf_t* ptr, int size)
{
    struct mytbf_st* me = ptr;

    if(size <= 0)
        return -EINVAL;
    
    pthread_mutex_lock(&me->mut);
    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;
    pthread_cond_broadcast(&me->cond);          //如果共用一个令牌桶，那么也需要通知
    pthread_mutex_unlock(&me->mut);

    return size;
}

int mytbf_destory(mytbf_t* ptr)
{
    //销毁指定的令牌，而不是把令牌桶全部销毁
    struct mytbf_st* me = ptr;

    pthread_mutex_lock(&mutjob);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mutjob);

    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(ptr);
}