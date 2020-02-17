#include "sched.h"


int main(int argc, char** argv){
  int no_of_processes;
  printf("Enter number of processes \n");
  scanf("%d", &no_of_processes);
  int choice;
  struct processes *p = malloc(sizeof(struct processes)*no_of_processes);
  
  for(int i = 0; i < no_of_processes; i++){
      printf("Enter process ID \n");
      scanf("%d",&p[i].process_id);
      printf("Enter Burst time \n");
      scanf("%d",&p[i].burst_time);
      printf("Enter arrival time \n");
      scanf("%d",&p[i].arrival_time);
  }

  while(choice!=5){
      printf("\t\t Main Menu \n");
      printf("\t 1. First Come First Serve(FCFS)\n");
      printf("\t 2. STCF (preemptive)\n");
      printf("\t 3. SJF (Non-Preemptive)\n");
      printf("\t 4. Priority Scheduling (Lottery)\n");
      printf("\t 5. Exit\n");
      printf("Enter choice \n");
      scanf("%d",&choice);
      switch(choice)
	{
	case 1: fcfs(p,no_of_processes);
	  break;

	case 2: stcf(p,no_of_processes);
	  break;

	case 3: sjf(p,no_of_processes);
	  break;

	case 4: lottery(p,no_of_processes);
	  break;
	  
	case 5: printf("Goodbye\n");
	  free(p);
	  exit(0);
	  
	default:
	  printf("Dont get that choice\n");
	  free(p);
	  exit(0);
	}
  }
  free(p);
  exit(0);
  
}
