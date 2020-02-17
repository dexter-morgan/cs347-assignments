#include "sched.h"

void fcfs(struct processes *p, int np)
{
  // Sort processes in order of arrival time.
  sort(p, np);
  int response_time = 0;  
  // clock
  int clock=0;
  clock += p[0].arrival_time;

  for(int i = 0; i < np; i++){
      p[i].response_time = clock - p[i].arrival_time;
      clock += p[i].burst_time;
      p[i].leave_time = clock;
      p[i].turnaround_time = p[i].leave_time - p[i].arrival_time;
      p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;

      if (clock < p[i+1].arrival_time)
        clock = p[i+1].arrival_time;
           
    }
  printf("ProcessId,ArrivalTime,BurstTime,Leaving Time, TurnaroundTime,WaitingTime, Response Time \n");
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


