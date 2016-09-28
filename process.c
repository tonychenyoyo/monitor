//process.c
#include "process.h"
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>

#define SIZE 1024
int exit_flag = 0;
struct proc_struct must_run_proc[SIZE];
int must_run_size;

/*********************************************************************
****check_proc
****参数array:进程描述结构体指针
**** 检测程序是否正在运行
**** 正在运行返回1，没有运行返回0
***********************************************************************/
int check_proc(struct proc_struct* array)
{
     DIR * dp;
     char * dir = PROCES;
     struct dirent * dirp;
     struct proc_struct* proc;
     struct proc_struct* tmp;
     if(!array)
     {
       return 0;
     }
	 //切换到目录/proc
     chdir(dir);
     if((dp = opendir(dir))== NULL) //打开目录/proc,失败返回0，成功把描述指针返回给dp
     {		 
		return 0;    
     }
 	 //将/proc文件夹的描述符指针传递给readdir,读取文件夹内容，循环赋值给结构体dirp
     while ((dirp = readdir(dp)) != NULL) 
     {
        char data[30];
		//获取文件名称赋值给数组data(其中包含有进程的名称(pid))
		sprintf(data, "%s", dirp->d_name); 
		//是否是由字符0-9组成的字符串，即得到所有进程的pid
		if((IsDigit(data)))
	    {
	        proc = (struct proc_struct *)(malloc(sizeof(struct proc_struct)));
            tmp = proc;                    
            proc->pid = atoi(dirp->d_name);
            if(proc_find(proc, array))
            {
                return 1;
            }
            free(tmp);   
        }
      }
      closedir(dp);
      return 0;
}


/*********************************************************************
****proc_find
****两个参数分别是两个进程描述的结构体指针
**** 比较两个进程pid是否相等
**** 相等返回1，不相等返回0
***********************************************************************/
int proc_find(struct proc_struct* src, struct proc_struct* dest)
{
    char buffer[4096], *p, cmd[20];
    int fd, len;
    sprintf(buffer, "%d/stat", src->pid);
    fd = open(buffer, O_RDONLY);
    if(fd == -1)
    {
     return 0;
    }
    memset(buffer, '\0', sizeof(buffer));
    len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);
    if(len <= 0)
    {
     return 0;
    }
    p = buffer;
    p = strrchr(p, '(');
    {
        char *q = strrchr(p, ')');
	    int len = q - p - 1;
	    if (len >= sizeof(src->name))
	    {
            len = sizeof(src->name)-1;
	    }
        memcpy(src->name, p + 1, len);
	    src->name[len] = 0;
	    p = q + 1;
    }
    return (strcmp(src->name, dest->name) == 0)? 1 : 0;
}

/*********************************************************************
****get_proc
****参数array:进程结构体指针;参数size:进程列表数组array的大小;file:配置文件路径
****从配置文件得到指定的程序列表，将对应进程的信息填充到array数组中
**** 执行成功返回进程个数，执行失败返回0
***********************************************************************/
int get_proc(struct proc_struct* array, int size, char* file)
{
     int nRet = 0;
     char *tmp;
     if(!array || (size <= 0) || !file)
     {
     	myprintf("invalid parameter\n");
        return 0;
   	 }
     char line[4096];
     FILE* fp = fopen(file, "r");
     if(!fp) 
     {
         printf("open file %s fail\n", file);
         return 0;
     }
     memset(line, 0, 4096);
     while(fgets(line, 4095, fp)&& nRet < size)
     {
         memcpy((void *)((&array[nRet])->cmdline), (void *)line, strlen(line) - 2 );
         tmp = strrchr(line, '/');
         tmp += 1;
         memcpy((&array[nRet])->name, tmp, strlen(tmp) - 2);
         nRet++;
     }
     fclose(fp);
     return (nRet);
}


/*********************************************************************
****startProc
****参数proc:要启动的进程的结构体描述指针
****启动程序
**** 执行成功返回1，子进程退出
***********************************************************************/
int startProc(struct proc_struct* proc,struct proc_struct *must_run_proc,int must_run_size)
{
    static int i = 0;
    if(!proc) return 0;
  	if(strlen(proc->cmdline) <= 0) return 0;
  
  	int pid = fork();
    //子进程内部返回值为0，返回给父进程的为自己的pid
    int a = 1;
    
    if(pid == 0)
        {
            pid = fork();
            if(pid == 0)
            {
                execl((char *)proc->cmdline, (char *)proc->name, NULL);
            //  exit(0);
            }
            else
            {
               exit(0);
            }
            sleep(2);
        }
        else
        {
            wait(NULL);
            sleep(1);
        }

   if(i == must_run_size - 1)
   {
      i = -1;
   }
   i++;
  if(check_proc(&must_run_proc[i]) == 0)
   {
      startProc(&must_run_proc[i], must_run_proc,must_run_size);
   }
   else
   {
        if(i == must_run_size - 1)
            i=0;
        else
        {
            i++;
            startProc(&must_run_proc[i], must_run_proc,must_run_size);
        }
   }

}


/*********************************************************************
**** daemon_init
**** 启动配置文件当中的需要守护的程序
****执行成功返回1，中途出错，返回-1 
***********************************************************************/
int moniter_run(struct proc_struct *must_run_proc, int proc_size)
{

	int i = 0;
    for(i = 0; i < must_run_size; i++)
    {
	 //监控程序是否正在运行
	if(check_proc(&(must_run_proc[i])) <= 0)
	{	//重新启动程序
	   startProc(&(must_run_proc[i]),must_run_proc, proc_size);
	}
    }
    return 1;
}
/////////////////////////////////////
/*********************************************************************
**** 信号处理函数exit_proc
**** 安全结束监控的程序
**** 没有返回值
***********************************************************************/
void exit_proc(int ar)
{
     int i;
     struct proc_struct* proc;
  
     for(i = 0; i < must_run_size; i++)
     {
          proc = &(must_run_proc[i]);
          kill(proc->pid, SIGTERM);
     }
	 exit_flag = 1;
     exit(0);  
	 
}
/////////////////////////////////////


void exit_proc(int pid);
/*********************************************************************
**** main
**** 1.获取程序列表2.启动进程 3.监控进程，若程序退出或崩溃，重新启动程序4.收到退出信号，安全结束监控程序
**** 成功返回1，失败返回 0
***********************************************************************/
static void* run_moniter(void *data)
{
    //读取程序列表
	must_run_size = get_proc(must_run_proc, SIZE, data);
	if(must_run_size <= 0)
	{
		return 0;
	}
    struct sigaction act, oldact;
    act.sa_handler = exit_proc;
    act.sa_flags = SA_ONESHOT;
	sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);

    //检测并启动未启动的程序
    moniter_run(must_run_proc, must_run_size);
	return NULL;
}

int creat_and_run_moniter(char *file)
{
//	开线程：
	pthread_t moniter_thread;
	if(pthread_create(&moniter_thread, NULL, run_moniter, file) == 0)
	{
		printf("thread create Ok, check thread start \n");
		return 0;
	}
	else
	{
		printf("thread check create Err\n");
		return -1;
	}
}

/*********************************************************************
****IsDigit
****参数a:字符串的地址
**** 判断字符串是否有纯数字字符组成
**** 是纯数字返回1，不是纯数字返回0
***********************************************************************/
static  int IsDigit(char a[])
{
        int size,i;
		//得到当前字符串的长度
        size = strlen(a);
		//若字符串长度为0，则直接返回0；即字符串为空则返回0；
        if(size == 0) return 0;
		//循环遍历整个字符串
        for(i = 0; i< size; i++)
        {
          //如果字符小于字符0，或者大于字符9，则返回0
          if(a[i] <'0' || a[i] > '9')
           {
             return 0;
           }
        }
		//走到这一步说明字符串由字符0-9组成，返回1
        return 1;
}


