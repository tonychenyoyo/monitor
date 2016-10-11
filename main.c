#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{

	char *file="proclist.ini";
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

	while(1)
	{
		sleep(1);
	}
	
    return 0;
}

