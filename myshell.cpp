#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <glob.h>
#define DELIMS " \t\n"

//这一步是为了记录外部命令额外的信息的
struct cmd_St
{
    glob_t globres;
};

static void prompt(void)
{
    std::cout << "mysh-0.1$";
}

static void parse(char* cmd, struct cmd_St* cmd_st)
{
    char *token;
    //glob_t globres;
    int flag = 0;

    while(1)
    {
        token = strsep(&cmd, DELIMS);   //分割命令语句
        if(token == NULL)
            break;
        if(token[0] == '\0')
            continue;
        //使用GLOB_NOCHECK是因为会返回最初的pattern
        glob(token, GLOB_NOCHECK|GLOB_APPEND*flag, NULL, &(cmd_st->globres));   //用glob来存储解析命令，除了第一次不要追加以外，后面都追加
        flag = 1;
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    char* linebuf = NULL;
    size_t linebuf_size = 0;
    struct cmd_St cmd_st;

    while(1)
    {
        prompt();

        //获取输入的命令语句
        if((getline(&linebuf, &linebuf_size, stdin) < 0))
            break;
        parse(linebuf, &cmd_st);

        if(0)
        {
            //内部命令
            //do sth
        }
        else
        {
            //外部命令
            pid = fork();
            if(pid < 0)
            {
                std::cerr << "fork wrong" << std::endl;
                exit(-1);
            }
            else if(pid == 0)
            {
                //子进程执行execvp，使用这个的原因是因为，普通的shell都是输入ls就可以查看，而不是/bin/ls这样的文件路径
                execvp(cmd_st.globres.gl_pathv[0], cmd_st.globres.gl_pathv);
                std::cerr << "execvp wrong" << std::endl;
                exit(-1);
            }
            else
            {
                //父进程回收资源
                wait(NULL);
            }
        }

    }


    return 0;
}
