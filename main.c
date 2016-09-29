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
	creat_and_run_moniter("proclist.ini");
	
	while(1)
	{
		sleep(1);
	}
	
    return 1;
}

