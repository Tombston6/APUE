#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#define THRNUM 20
#define FILENAME "/tmp/out"
#define SIZE 1024

//该程序演示了线程竞争

static void* thr_add(void* arg)
{
    FILE* fp;
    char linebuf[SIZE];
    fp = fopen(FILENAME, "r+");
    assert(fp != NULL);
    fgets(linebuf, SIZE, fp);
    fseek(fp, 0, SEEK_SET);             //文件的指针定位
    fprintf(fp, "%d\n", (atoi(linebuf)+1));
    fclose(fp);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid[THRNUM];
    int err;

    for(int i = 0 ; i < THRNUM; i++)
    {
        err = pthread_create(&tid[i], NULL, thr_add, NULL);
        assert(err == 0);
    }

    for(int i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;
}