#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#define __NR_freeblocks 436

long freeblocks(char* path, uint64_t *freeblocks_num) {
	return syscall(__NR_freeblocks, path, freeblocks_num);
}

int main()
{
	uint64_t freeblocks_num;
	char * path = "/root/.bashrc";
	long res = freeblocks(path, &freeblocks_num);

	printf("r:%ld d: %lld\r\n", res, freeblocks_num);
}

