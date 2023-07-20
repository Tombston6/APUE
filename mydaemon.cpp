#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

//由于是守护进程，所以不会有终端，所以要与标准输入输出相关
#define FNAME "/tmp/out"

static void daemonize(void)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        syslog(LOG_ERR, "fork wrong");
        exit(-1);
    }
    else if(pid > 0)
    {
        //parent
        exit(0);
    }
    else
    {
        //child
        //要脱离控制终端，所以文件描述符0，1，2不能连接终端
        int fd;
        fd = open("/dev/null", O_RDWR);
        if(fd < 0)
        {
            syslog(LOG_ERR, "open wrong");
            exit(-1);
        }

        //重定向
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        if(fd > 2)
            close(fd);

        //做成守护进程
        setsid();

        //收尾工作
        //如果这个守护进程是在踩着一个设备运行，比如某个U盘，但是又要umount盘的时候，就无法正常umount了，这时候就需要chdir
        chdir("/"); //将工作目录设置为根目录
        //umask(0);   //如果确定程序不会再产生文件了，就可以关掉umask值
    }

}

int main(void)
{
    FILE* fp;
    int i;
    openlog("mydaemon", LOG_PID, LOG_DAEMON);   //使用系统日志文件来打印出错消息，不用使用终端的0，1，2

    daemonize();
    fp = fopen(FNAME, "w");
    if(fp == NULL)
    {
        syslog(LOG_ERR, "fopen is wrong");
        exit(-1);
    }

    for(i = 0; ; i++)
    {
        fprintf(fp, "%d\n", i);
        fflush(fp);
        sleep(1);
    }
    fclose(fp);
    closelog();
    return 0;
}
