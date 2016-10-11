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
#include "vns_list.h"
#include "lib/iniparser.h"

static struct list_head g_process_head= LIST_HEAD_INIT(g_process_head);

static  int IsDigit(char a[])
{
        int size,i;

        size = strlen(a);
	
        if(size == 0) return 0;
	
        for(i = 0; i< size; i++)
        {
          if(a[i] <'0' || a[i] > '9')
           {
             return 0;
           }
        }
        return 1;
}


int check_proc(struct process_info* proc_dest)
{
	DIR * dp;
	char * dir = PROCES;
	struct dirent * dirp;
	struct process_info* proc_src;
	//struct process_info* tmp;
	if(!proc_dest)
	{
		return 0;
	}
	chdir(dir);
     	if((dp = opendir(dir))== NULL) 
     	{		 
		return 0;    
     	}
	while ((dirp = readdir(dp)) != NULL) 
	{
		char data[30]={0};
		sprintf(data, "%s", dirp->d_name); 
		
		if(IsDigit(data))
		{
			printf("%s, ",data);
			proc_src = (struct process_info *)(malloc(sizeof(struct process_info)));
			//tmp = proc_src;                    
			proc_src->pid = atoi(dirp->d_name);//pid找出来
			if(proc_find(proc_src, proc_dest))
			{
				return 1;
			}
			free(proc_src);   
		}
	}
//	printf("\n");

	
}

//比较系统的进程和目标进程，SRC指代系统实时进程
int proc_find(struct process_info* src, struct process_info* dest)
{
    char buffer[MAX_SIZE*10], *p, cmd[MAX_SIZE];
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

//遍历链表，目前仅用于调试
void traverse(struct list_head* ghead)
{
	struct list_head* pos;
	struct list_head* n;
	struct process_info *proc;
	list_for_each_safe(pos,n,ghead)
	{
			proc=list_entry(pos,struct process_info,list);
			printf("%s ,%s\n",proc->name,proc->cmdline);
	}
}


 
//把配置文件上的监控进程填入链表中，
int get_proc(struct list_head* ghead,char* file)
{
	int i;
	dictionary	*	d ;
	d=iniparser_load(file);
	//dictionary_dump(ini,stdout);
	for (i=0 ; i<d->size ; i++) 
	{
	        if (d->key[i]&&d->val[i]) 
		{
			struct process_info *p=(struct process_info*)malloc(sizeof(struct process_info));
			if(p==NULL)
			{
				printf("malloc process_info fail\n");
			}

			memset(p,0,sizeof(struct process_info));
		
			memcpy(p->name,d->key[i],strlen(d->key[i]));
			memcpy(p->cmdline,d->val[i],strlen(d->val[i]));
			printf("====================\n");
			//printf("%s ,%s\n",p->name,p->cmdline);
			
			list_add(&p->list,ghead);

			traverse(ghead);
	                   
	        }
	}
	
}


int startProc(struct process_info* proc)
{
	if(!proc) 
		return 0;
	if(strlen(proc->cmdline) <= 0) 
		return 0;

	FILE *fp;
	fp = popen(proc->cmdline, "r");
	if(fp == NULL){
		return 0;
	}
	pclose(fp);
	return 1;

}

int moniter_process(struct list_head *ghead)
{

	struct list_head* pos;
	struct list_head* n;
	struct process_info *proc;
//遍历配置文件中需要监控的进程
	list_for_each_safe(pos,n,ghead)
	{
			proc=list_entry(pos,struct process_info,list);
			if(check_proc(proc)<=0)//这个程序没有在运行
			{
				startProc(proc);//启动这个程序
				
			}
	}
	return 1;
}

void exit_proc(int ar)
{
	struct list_head* pos;
	struct list_head* n;
	struct process_info *proc;

	list_for_each_safe(pos,n,&g_process_head)
	{
			proc=list_entry(pos,struct process_info,list);
			 kill(proc->pid, SIGTERM);
			 list_del(&proc->list);
			 free(proc);
	}

}

void* run_moniter(void *data)
{
	int must_run_size;
	must_run_size=get_proc(&g_process_head,data);
	if(must_run_size <= 0)
	{
		return 0;
	}
	/*
	struct sigaction act, oldact;
	act.sa_handler = exit_proc;
	act.sa_flags = SA_ONESHOT;
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGHUP, &act, NULL);
	*/
	moniter_process(&g_process_head);
	return NULL;
}



