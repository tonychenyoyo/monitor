//process.h
#ifndef _PROCESS_H_
#define _PROCESS_H_
#define PROCES "/proc"
#define MAX_PROC_NAME 128
#define MAX_CMD_LINE 256
#define PROC_STATUS 6
#define INVALID_INTEGER -1
#define INVALID_ULONG 0
#define INVALID_DOUBLE 0
#define DEBUG 0
#define SIZE 1024


#if DEBUG
#define myprintf(fmt, args...)    printf("%s,%s(),%d:" fmt "\n", __FILE__,__FUNCTION__,__LINE__, ##args)
#else 
#define myprintf(fmt, args...)    
#endif


//进程结构体定义
struct proc_struct
{
 	char name[MAX_PROC_NAME];
 	char cmdline[MAX_CMD_LINE];
 	int pid;
};
int check_proc(struct proc_struct* array);
int proc_find(struct proc_struct* proc1,struct proc_struct* proc);
int get_proc(struct proc_struct* array, int size, char* file);
int startProc(struct proc_struct* proc,struct proc_struct *must_run_proc,int must_run_size);
int creat_and_run_moniter(char *file);
static  int IsDigit(char a[]);
#endif

