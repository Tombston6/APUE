#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id) % NHASH)

struct foo* fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo
{
    int f_count;
    pthread_mutex_t f_lock;
    int f_id;
    struct foo * f_next;        //通过hashlock来保护
};

struct foo* foo_alloc(int id)
{
    /*初始化互斥量结构体*/
    struct foo *fp;
    int idx;

    if((fp = (struct foo*)malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 1;
        fp->f_id = id;
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return NULL;
        }
        idx = HASH(id);             //散列
        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
    }
    return fp;
}

void foo_hold(struct foo* fp)
{
    //增加引用对象
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

struct foo* foo_find(int id)
{
    //寻找已经存在的对象
    struct foo* fp;
    pthread_mutex_lock(&hashlock);
    for(fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next)
    {
        if(fp->f_id == id)
        {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(struct foo* fp)
{
    //释放一个引用

    struct foo *tfp;
    int idx;

    pthread_mutex_lock(&fp->f_lock);
    if(fp->f_count == 1)
    {
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);
        
        //再次检查是否符合条件，因为可能在上一次被阻塞
        if(fp->f_count != 1)
        {
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return;
        }

        //从链表中移除
        idx = HASH(fp->f_id);
        tfp = fh[idx];
        if(tfp == fp)
            fh[idx] = fp->f_next;
        else
        {
            while(tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
    }
}