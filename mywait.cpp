#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <iostream>
#define LEFT 2000
#define RIGHT 2300
//交叉分配法

int main(void)
{
	pid_t pid;
	int i, j;

	//创建三个进程来获取偶数
	for(int i = 0; i < 3; i++)
	{
		if((pid = fork()) < 0)
		{
			//可继续优化，应该在生成子进程失败之后回收资源
			std::cerr << "fork()" << std::endl;
			exit(-1);
		}
		else if(pid == 0)
		{
			for(j = LEFT + i; j <= RIGHT; j += 3)
			{
				if(j % 2 == 0)
					std::cout << "[" << getpid() << "]:" << j << "是偶数" << std::endl;
			}
			exit(0);
		}
	}
	
	//回收资源 防止僵尸进程的产生
	for(int i = 0; i < 3; i++)
		wait(NULL);
	exit(0);
}
