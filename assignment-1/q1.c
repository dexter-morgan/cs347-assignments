#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


char **tokenize(char *input);

char **return_u_and_s(char *pid){
    char **u_and_s = (char **) malloc(3 * sizeof(char *));
    FILE *fp;
   char str[30];
   int count = 0;

   char location[40] = "/proc/";
    strcat(location, pid);
    strcat(location,"/stat");
   
   fp = fopen( location, "r");
   if(fp == NULL) {
      perror("Error opening file");
      return(-1);
   }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    

    if ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        char **stats = tokenize(line);
        //printf("%s %s\n",stats[13],stats[14] );
        u_and_s[0] = stats[13];
        u_and_s[1] = stats[14];
    }

    fclose(fp);
    if (line)
        free(line);
    
    u_and_s[2] = NULL;
    int j =0;
    return u_and_s;
}

int return_cpu_sum(){
    int cpu_sum = 0;
    FILE *fp;
   char str[30];
   int count = 0;

   char location[40] = "/proc/";
    strcat(location,"stat");
   
   fp = fopen( location, "r");
   if(fp == NULL) {
      perror("Error opening file");
      return(-1);
   }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    

    if ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s\n", line );
        char **stats = tokenize(line);
        //printf("%d %s\n",atoi(stats[2]),stats[1]);
        int j = 0;
        while (stats[j]!=NULL){
        cpu_sum += atoi(stats[j]);
        j++;
        }
        //printf("%d\n", cpu_sum );
    }


    fclose(fp);
    if (line)
        free(line);
    
    return cpu_sum;
}


void solve_q1(char **words){
	int utime_before, utime_after;
    int stime_before, stime_after;
    int total_time_before, total_time_after;

    char **u_and_s = return_u_and_s(words[1]);
    utime_before = atoi(u_and_s[0]);
    stime_before = atoi(u_and_s[1]);
    total_time_before = return_cpu_sum();
    
    sleep(5);
    
    u_and_s = return_u_and_s(words[1]);
    utime_after = atoi(u_and_s[0]);
    stime_after = atoi(u_and_s[1]);
    total_time_after = return_cpu_sum();
    free(u_and_s);

    int ut = 100 * (utime_after - utime_before) / (total_time_after - total_time_before);
    int st = 100*(stime_after - stime_before) / (total_time_after - total_time_before);
	printf("User Mode CPU Percentage: %d\n", ut);
    printf("System Mode CPU Percentage: %d\n", st);
    exit(0);
}
