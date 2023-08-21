//当使用结构体变量为自动变量作为pthread_exit的参数时出现的问题
//为了避免这种情况，应该使用全局结构或者用malloc分配结构

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct foo
{
    int a, b ,c, d;
};

void printfoo(const char* s, const struct foo *fp)
{
    printf("%s", s);
    printf(" structure at 0x%lx\n", (unsigned long)fp);
    printf(" foo.a = %d\n", fp->a);
    printf(" foo.b = %d\n", fp->b);
    printf(" foo.c = %d\n", fp->c);
    printf(" foo.d = %d\n", fp->d);
}

void* thr_fn1(void *arg)
{
    struct foo f = {1, 2, 3, 4};
    printfoo("thread 1:\n", &f);
    pthread_exit((void*)&f);
}

void* thr_fn2(void* arg)
{
    printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
    pthread_exit((void*)0);
}

int main(void)
{
    int err;
    pthread_t tid1, tid2;
    struct foo *fp;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if(err != 0)
    {
        perror("pthread_create wrong");
        exit(1);
    }
    err = pthread_join(tid1, (void**)&fp);
    if(err != 0)
    {
        perror("pthread_join wrong");
        exit(1);
    }
    sleep(1);

    printf("parent starting second thread\n");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if(err != 0)
    {
        perror("pthread_create wrong");
        exit(1);
    }
    sleep(1);
    printfoo("parent:\n", fp);

    return 0;
}