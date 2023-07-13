#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <stdio.h>
//该程序即保留了壳，pid的值是没有变化的

int main(int argc, char** argv)
{
    puts("Begin!");
    fflush(NULL);   //刷新缓冲区

    int exec_val;

    //需要注意的是，如果execl成功，那么程序就区执行date程序了，即不会执行下面的std::cerr及以下代码
    exec_val = execl("/usr/bin/date", "date", "+%s", NULL);
    std::cerr << "execl wrong" << std::endl;
    exit(1);

    std::cout << "end" << std::endl;

    return 0;
}
