#define MAX_CPU_USE 0.8
#define MAX_MEMORY_USE 0.8
#define MAX_FLASH_USE 0.6
/*

user 
 从系统启动开始累计到当前时刻，用户态的CPU时间，不包含 nice值为负进程。
 
nice 
 从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
 
system 
 从系统启动开始累计到当前时刻，核心时间
 
idle 
 从系统启动开始累计到当前时刻，除IO等待时间以外其它等待时间
 
iowait
 从系统启动开始累计到当前时刻，IO等待时间
 
irq 
 从系统启动开始累计到当前时刻，硬中断时间
 
softirq 
 从系统启动开始累计到当前时刻，软中断时间

*/

typedef struct{
	unsigned int user;
	unsigned int system;
	unsigned int nice;
	unsigned int idle;
	unsigned int iowait;
	unsigned int hardirq;
	unsigned int softirq;
	unsigned int st;
}proc_stat_t;

typedef struct tag_cmts_system{
	unsigned char cpu;
	unsigned char memory;
	unsigned char flash;
	unsigned char reserved;
/*
    unsigned char min_mem;
    unsigned char max_mem;
    unsigned int min_mem_time;
    unsigned int max_mem_time;

    unsigned char min_cpu;
    unsigned char max_cpu;
    unsigned int min_cpu_time;
    unsigned int max_cpu_time;

    unsigned char min_flash;
    unsigned char max_flash;
    unsigned int min_flash_time;
    unsigned int max_flash_time;
 */
}cmts_system;

int check_system(cmts_system *sys);




