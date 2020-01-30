#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>


void solve_q4(char **words){
	int fd = dup(0);
	close(0);
	int new_fd = open("./sort.txt", O_RDONLY, S_IRWXU);
	int rc2 = fork();
	if (rc2 == 0){
		char *myargs[2];
		myargs[0] = "sort";
		myargs[1] = NULL;
		execv("/usr/bin/sort",myargs);
	}
	else{
		wait(NULL);
		close(new_fd);
		dup2(fd,0);
	}
}