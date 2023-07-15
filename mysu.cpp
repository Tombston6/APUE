#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    pid_t pid;

    if(argc < 3)
    {
        std::cerr << "param is too few" << std::endl;
        exit(-1);
    }
    pid = fork();
    if(pid < 0)
    {
        std::cerr << "fork失败" << std::endl;
        exit(-1);
    }
    else if(pid == 0)
    {
        setuid(atoi(argv[1]));      //设置uid号
        execvp(argv[2], argv+2);    //执行程序
        std::cerr << "execvp wrong" << std::endl;
        exit(-1);
    }
    wait(NULL);                     //回收资源

    return 0;
}

//执行时，需要将该执行文件归属者设为root，同时设置为u+s全限制
