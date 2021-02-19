// #define _GNU_SOURCE
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define __NR_mysyscall 332

long mysyscall(void) {
    syscall(__NR_mysyscall);
}

int main() {
    long result = mysyscall();
    printf("Result: %d\n", result);

    return 0;
}
