#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define __NR_kernelps 435
#define CMD_SIZE 200

long kernelps(size_t * task_cnt, char** arr) 
{
	return syscall(__NR_kernelps, task_cnt, arr);
}

int main(int argc, char* argv[])
{
	size_t task_cnt;
	size_t task_written;
	if(argc != 2){
		printf("ERROR");
		return 0;
	}
	sscanf(argv[1], "%d", &task_cnt);
	char ** arr  = (char**) malloc(sizeof(char*) * task_cnt);
	
	for(int i = 0; i < task_cnt; i++){
		arr[i] = malloc(CMD_SIZE);
		arr[i][CMD_SIZE-1] = '\0';
	}
	arr[task_cnt-1] = NULL;
	task_written = task_cnt;
	long res = kernelps(&task_written, arr);
    printf("task_written %d\r\n", task_written);
	for(int i = 0; i < task_written; i++){
            printf("%s\n", arr[i]);
        
    }
	printf("clear memory\r\n");
    for(int i = 0; i < task_cnt; i++){
        free(arr[i]);
    }
	free(arr);
}

