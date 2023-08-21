#ifndef __MYSEM_H__
#define __MYSEM_H__
//使用条件变量和互斥量完成一个有限的能够记次数的资源共享

typedef void mysem_t;

mysem_t* mysem_init(int initval);
int mysem_add(mysem_t*, int);
int mysem_sub(mysem_t *, int);
int mysem_destory(mysem_t* );

#endif