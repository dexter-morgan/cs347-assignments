#include "sched.h"
#include <limits.h>
#include<stdlib.h>

int next_process_lottery(struct processes *p, int np, int clock)
{
  int next_pid;
  int j, counter = 0;

  int winner = rand()%100;

  for(j = 0; j < np; j++){

    counter += p[j].priority;
    if (counter>winner) {
      next_pid = j;
      break; 
      }
    }

    // printf(" next pid : %d\n", next_pid );

  return next_pid;
}

int completed(struct processes *p, int np);

void lottery(struct processes *p, int np)
{

  for(int i = 0; i < np; i++){
      printf("Enter priority for process ID  %d (out of 100) :\n", p[i].process_id);
      scanf("%d",&p[i].priority);
  }

  int time_slice;
  printf("Enter time slice length : \n");
  scanf("%d", &time_slice);

  // Sort processes in order of arrival time.
  sort(p, np);
  int *winning_count = malloc(sizeof(int) * np); 

  for(int i = 0; i<np; i++){
    p[i].finished = 0;
    p[i].remaining_time = p[i].burst_time;
    p[i].response_time = -1;
    winning_count[i]=0;
  }

  int response_time = 0;
  

  // clock
  int clock=0;
  clock += p[0].arrival_time;

  while(!completed(p, np)){

      // printf(" current clock moment : %d\n", clock);
      int next_pid = next_process_lottery(p, np, clock);
      winning_count[next_pid]++;
      if(p[next_pid].finished==1) continue;

      if(p[next_pid].response_time == -1) p[next_pid].response_time = clock - p[next_pid].arrival_time;

      if (p[next_pid].remaining_time>time_slice)
      {
        p[next_pid].remaining_time = p[next_pid].remaining_time - time_slice;
        clock += time_slice;
        continue;  
      }
      else {
        clock += p[next_pid].remaining_time;
        p[next_pid].remaining_time = 0;
        p[next_pid].leave_time = clock;
        p[next_pid].turnaround_time = p[next_pid].leave_time - p[next_pid].arrival_time;
        p[next_pid].waiting_time = p[next_pid].turnaround_time - p[next_pid].burst_time;
        p[next_pid].finished = 1;
      }  

        
      for(int j = 0; j < np; j++){
        if (p[j].finished!= 1) {
          if(clock > p[j].arrival_time){
            break;
          }
          else{
            clock = p[j].arrival_time;
            break;
          }
        }
      }

  }
      
  int sum=0;
  for(int j = 0; j < np; j++){
    sum+=winning_count[j];
  }

  for(int j = 0; j < np; j++){
    printf("The fraction of time for process %d is : %f \n", p[j].process_id, (float)winning_count[j]/sum );
  }


    
  printf("\n ProcessId,ArrivalTime,BurstTime,Leaving Time, TurnaroundTime,WaitingTime, Response Time \n");
  for(int i = 0; i < np; i++){
    printf("%d,%d,%d,%d,%d,%d,%d\n",
     p[i].process_id,
     p[i].arrival_time,
     p[i].burst_time,
     p[i].leave_time,
     p[i].turnaround_time,
     p[i].waiting_time,
     p[i].response_time);
  }
  int t_wait_time = 0, t_turn_time = 0, t_resp_time = 0;
  
  for(int i=0; i<np;i++){
    t_wait_time += p[i].waiting_time;
    t_turn_time +=p[i].turnaround_time;
    t_resp_time +=p[i].response_time;
  }
  float avg_wait = t_wait_time /(float)np;
  float avg_turn = t_turn_time /(float)np;
  float avg_resp = t_resp_time /(float)np;
  printf("Average waiting time : %f \n",avg_wait);
  printf("Average turnaround time : %f \n",avg_turn);
  printf("Average response time : %f \n",avg_resp);
}

