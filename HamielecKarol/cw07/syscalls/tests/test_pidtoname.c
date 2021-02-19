#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define __NR_pidtoname 437
#define TASK_NAME_SIZE 300
long pidtoname(pid_t pid, char * str) {
	return syscall(__NR_pidtoname, pid, str);
}

int main(int argc, char* argv[])
{
	pid_t pid;

	if(argc != 2){
		printf("ERROR");
		return 0;
	}
	sscanf(argv[1], "%d", &pid);
	char task_name[TASK_NAME_SIZE];
	for(int i = 0; i < TASK_NAME_SIZE; i++){
		task_name[i] = '\0';
	}
	task_name[TASK_NAME_SIZE-1] = '\0';
	long res = pidtoname(pid, task_name);
	printf("pid:%d r:%ld d: %s\r\n", pid, res, task_name);


	return 0;
}
