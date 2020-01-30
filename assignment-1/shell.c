#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char **tokenize(char *line);
void solve_q1(char **words);
void solve_q2(char **words);
void solve_q3(char **words);
void solve_q4(char **words);
//void solve_q5(char **words);*/

int main()
{
    char input[1024];
    while (1)
    {
        printf("myshell> ");
        memset(input, 0, sizeof(input));   
        gets(input);
        char **words = tokenize(input);
        //printf("%s\n",words[0] );
        /** WRITE YOUR CODE HERE **/

        if (strstr(input, ""))

        int rc = fork();
        if (rc < 0){
        	fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0){
        	if (strcmp(words[0],"checkcpupercentage") == 0){
        		solve_q1(words);
        	}else if (strcmp(words[0],"checkresidentmemory") == 0){
        		solve_q2(words);
        	}else if (strcmp(words[0],"listFiles") == 0){
        		solve_q3(words);
        	}else if (strcmp(words[0],"sort") == 0){
        		solve_q4(words);
        	}
        	else {
        		printf("invalid n" );
        	}

        }
        else {
        	wait(NULL);
        }
    }
    return 0;
}


char **tokenize(char *input)
{
    int count = 0;
    char **tokens = (char **) malloc(64 * sizeof(char *));
    char *sch;
    sch = strtok(input, " \t\n"); 

    while (sch != NULL)
    {   
        tokens[count] = sch;
        count++;
        sch = strtok(NULL, " \t\n"); 
    }

    free(sch);
    tokens[count] = NULL ;
    return tokens;
}