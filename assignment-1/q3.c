#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void solve_q3(char **words){
	int fd = dup(0);
	open("./File.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	char *myargs[2];
	myargs[0] = "ls";
	myargs[1] = NULL;
	execve("/usr/bin/ls",myargs);
}