#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void solve_q2(char **words){
	char *myargs[] = {"ps", "-ho", "rss", words[1], 0};
	execvp(myargs[0],myargs);
} 