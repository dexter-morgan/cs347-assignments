#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

void solve_q3(char **words)
{
	int fd = dup(1);
	close(1);
	int new_fd = open("./File.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	int rc2 = fork();
	if (rc2 == 0)
	{
		char *myargs[2];
		myargs[0] = "ls";
		myargs[1] = NULL;
		execv("/usr/bin/ls", myargs);
	}
	else
	{
		wait(NULL);
		fflush(stdout);
		close(new_fd);
		dup2(fd, 1);
	}
}