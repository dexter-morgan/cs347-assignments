#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char **tokenize(char *line);
char **tokenize_multiple(char *line);

void solve_q1(char **words);
void solve_q2(char **words);
void solve_q3(char **words);
void solve_q4(char **words);

void execute_child(char **words);

int main()
{
	char input[1024];
	char input2[1024];
	while (1)
	{
		printf("myshell> ");
		memset(input, 0, sizeof(input));
		gets(input);
		strcpy(input2, input);
		printf("\n\ninput 2 : %s \n input 1 :  %S \n\n\n", input2, input);
		char **words = tokenize(input);
		// printf("\n\n\n after tokenize : input 2 : %s \n input 1 :  %S \n", input2, input);
		/** WRITE YOUR CODE HERE **/
		// printf("here is the input command : %s", input);
		char **commands = tokenize_multiple(input2);
		int m = 0;
		// while (commands[m] != NULL)
		// {
		// 	printf("%s \n", commands[m]);
		// 	m++;
		// }

		if (strcmp(words[0], "exit") == 0)
		{
			printf("See you Again\n");
			exit(0);
		}

		int j = 0, k = 0, i = 0;
		int flag = 0;

		while (words[j] != NULL)
		{
			if (strcmp(words[j], ";") == 0)
			{
				flag = 1;
				break;
			}
			j++;
		}

		if (flag == 1)
		{
			char **cmd1 = (char **)malloc(64 * sizeof(char *));
			char **cmd2 = (char **)malloc(64 * sizeof(char *));

			for (i = 0; i < j; i++)
			{
				cmd1[i] = words[i];
			}

			cmd1[j] = NULL;

			for (k = j + 1; words[k] != NULL; k++)
			{
				cmd2[k - j - 1] = words[k];
			}

			cmd2[k - j - 1] = NULL;

			int pids[2];

			for (i = 0; i < 2; i++)
			{
				pids[i] = fork();
				if (pids[i] == 0)
				{
					if (i == 0)
						execute_child(cmd1);
					if (i == 1)
						execute_child(cmd2);
					exit(0);
				}
			}

			for (i = 0; i < 2; i++)
			{
				wait(NULL);
			}
		}

		else
		{
			int rc = fork();

			if (rc < 0)
			{
				fprintf(stderr, "fork failed\n");
				exit(1);
			}
			else if (rc == 0)
			{
				execute_child(words);
				exit(0);
			}
			else
			{
				wait(NULL);
			}
		}
	}
	return 0;
}

char **tokenize(char *input)
{
	int count = 0;
	char **tokens = (char **)malloc(32 * sizeof(char *));
	char *sch;
	sch = strtok(input, " \t\n");

	while (sch != NULL)
	{
		tokens[count] = sch;
		count++;
		sch = strtok(NULL, " \t\n");
	}

	free(sch);
	tokens[count] = NULL;
	return tokens;
}

char **tokenize_multiple(char *input)
{
	int count = 0;
	char **tokens = (char **)malloc(32 * sizeof(char *));
	char *sch;
	sch = strtok(input, ";");

	while (sch != NULL)
	{
		printf("%s\n", sch);
		tokens[count] = sch;
		count++;
		sch = strtok(NULL, ";");
	}

	free(sch);
	tokens[count] = NULL;
	return tokens;
}

void execute_child(char **words)
{

	if (strcmp(words[0], "checkcpupercentage") == 0)
	{
		solve_q1(words);
	}
	else if (strcmp(words[0], "checkresidentmemory") == 0)
	{
		solve_q2(words);
	}
	else if (strcmp(words[0], "listFiles") == 0)
	{
		solve_q3(words);
	}
	else if (strcmp(words[0], "sort") == 0)
	{
		solve_q4(words);
	}
	else
	{
		printf("invalid \n");
	}
}