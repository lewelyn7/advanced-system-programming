#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define __NR_topuser 434

long topuser(uid_t *uid) {
	syscall(__NR_topuser, uid);
}

int main()
{
	uid_t user;
	long result = topuser(&user);
	printf("r: %d u: %d\n", result, user);
}

