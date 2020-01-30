#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void solve_q2(char **words){
	char *myargs[5];
	myargs[0] = "ps";
	myargs[1] = "-ho";
	myargs[2] = "rss";
	myargs[3] = words[1];
	myargs[4] = NULL;
	execve("/usr/bin/ps",myargs);
} 