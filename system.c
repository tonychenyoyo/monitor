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
#include "system.h"

static proc_stat_t total_stat[2];
static char cpu_stat_swap = 0;

static unsigned int statistics_cpu_use(proc_stat_t *total_stat)
{
	unsigned int cpu_use_stat = total_stat->user
						 + total_stat->system
						 + total_stat->idle
						 + total_stat->nice
						 + total_stat->iowait
						 + total_stat->hardirq
						 + total_stat->softirq
						 + total_stat->st;
	return cpu_use_stat;
}

// 获取CPU状态

static int get_cpu_stat(proc_stat_t *total_stat)
{
	int len = 0;
	char str[200] = {0};
	char *token;
	int idx=0;
	int fd = open("/proc/stat",O_RDONLY);
	if (fd < 0) {
		printf("open /proc/stat fail.\n");
		return -1;
	}
	len = read(fd, str, sizeof(str)-1);
	if(len <= 0){
		close(fd);
		printf("read /proc/stat fail.\n");
		return -1;
	}
	str[len] = 0;
	close(fd);

	token = strtok(str, " ");
	while(token != NULL){
	switch(idx){
	case 1:
		total_stat->user = strtoul(token,NULL,10);//用户态时间
		break;
	case 2:
		total_stat->system = strtoul(token,NULL,10);//核心时间
		break;
	case 3:
		total_stat->nice = strtoul(token,NULL,10);//nice时间
		break;
	case 4:
		total_stat->idle = strtoul(token,NULL,10);//idle空闲时间
		break;
	case 5:
		total_stat->iowait = strtoul(token,NULL,10);//io等待时间
		break;
	case 6:
		total_stat->hardirq= strtoul(token,NULL,10);//硬中断时间
		break;
	case 7:
		total_stat->softirq= strtoul(token,NULL,10);//软中断时间
		break;
	}
	token = strtok(NULL, " ");
	idx++;
	}
	return 0;

}

//计算CPU使用率
static unsigned char calc_cpu_usage(void)
{
	int cpu_usage = 0;
	unsigned int cpu_id_diff = 0;
	unsigned int cpu_use_stat_0;
	unsigned int cpu_use_stat_1;
	unsigned int cpu_diff = 0;
	float cpu_id_f;

	cpu_use_stat_0 = statistics_cpu_use(&total_stat[(cpu_stat_swap - 1) % 2]);
	cpu_use_stat_1 = statistics_cpu_use(&total_stat[cpu_stat_swap % 2]);

	cpu_diff = cpu_use_stat_1 - cpu_use_stat_0;//两者之差
	cpu_id_diff = total_stat[cpu_stat_swap % 2].idle- total_stat[(cpu_stat_swap - 1) % 2].idle;
	if (cpu_diff) {
		cpu_id_f = ((float)cpu_id_diff / (float)cpu_diff);
		cpu_usage = 100*(cpu_diff - cpu_id_diff)/(float)cpu_diff;//减去空闲的CPU使用
	}
	return (cpu_usage);
}


//执行命令行
static int vns_cmd_qx(const char *cmdstring, char *out, unsigned int size)
{
	FILE *fp = popen(cmdstring, "r");
	if ( !fp ){
		return -1;
	}else if ( !out ){
	}else{
		char buf[1024]={0};
		unsigned int sz = 0;
		while(fgets(buf, sizeof(buf), fp) != NULL){
			sz += snprintf(out + sz, size - sz, "%s", buf);
			if(size <= sz + 1){
				//make sure out buffer is big enough
				pclose(fp);
				return -2;
			}
		}
	}
	pclose(fp);
	return 0;
}



//获取内存使用
static unsigned char get_cmts_mem(void)
{
	char buf[32]={0};
	int MemTotal = 0;
	int MemFree  = 0;
	int Buffers = 0;
	int Cached = 0;

	//MemTotal
	vns_cmd_qx("cat /proc/meminfo | awk '/MemTotal/ {print $2}'", buf, sizeof(buf));
	MemTotal = strtoul(buf, NULL, 0);
	//MemFree
    memset(buf, 0, sizeof(buf));
	vns_cmd_qx("cat /proc/meminfo | awk '/MemFree/ {print $2}'", buf, sizeof(buf));
	MemFree = strtoul(buf, NULL, 0);
	//Buffers
	memset(buf, 0, sizeof(buf));
	vns_cmd_qx("cat /proc/meminfo | awk '/Buffers/ {print $2}'", buf, sizeof(buf));
	Buffers = strtoul(buf, NULL, 0);
	//Cached
	memset(buf, 0, sizeof(buf));
	vns_cmd_qx("cat /proc/meminfo | awk '/Cached/ {print $2}'", buf, sizeof(buf));
	Cached = strtoul(buf, NULL, 0);

	return ((MemTotal - MemFree - Buffers - Cached)*100/MemTotal);
}

//获取FLASH状态
 static unsigned char get_cmts_flash(void)
{
	char *p;
	char buf[16];

	vns_cmd_qx("df / | tail -n 1 | awk '{print $5}'", buf, sizeof(buf));
	p = strchr(buf, '%');
	if(p != NULL){
		*p = '\0';
	}
	return (strtoul(buf, NULL, 0));
}

//获取内存，FLASH，CPU使用情况
int check_system(cmts_system *sys)
{

	int ret;
	// 1 get cpu usage
	ret = get_cpu_stat(&total_stat[cpu_stat_swap % 2]);
	if (!ret) {
		// 只有cpu_stat_swap为1的时候才计算CPU使用率
		if (cpu_stat_swap) 
		{
			sys->cpu = calc_cpu_usage();
			if(sys->cpu>MAX_CPU_USE)
			{
				//超出报警写日志
				//d1();
			}
		}
		cpu_stat_swap++;
		if (cpu_stat_swap > 1)
			cpu_stat_swap = 0;
	}

	// 2 get memory usage
	sys->memory = get_cmts_mem();
	if(sys->memory>MAX_MEMORY_USE)
	{
		//超出报警，写日志
		//d1();
	}

	// 3 get flash usage

	sys->flash = get_cmts_flash();
	if(sys->flash>MAX_FLASH_USE)
	{
		//超出报警，写日志
		//d1();
	}

	return 0;

}




