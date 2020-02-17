#include "sched.h"
#include <limits.h>

int next_process_stcf(struct processes *p, int np, int clock)
{
  int min_remaining_time = INT_MAX;
  int next_pid;
  int j;
  for(j = 0; j < np; j++){
        if(p[j].arrival_time > clock) break;
        if(p[j].arrival_time <= clock && p[j].remaining_time < min_remaining_time && p[j].finished!=1){
          next_pid = j;
          min_remaining_time = p[j].remaining_time;
        }
      }printf(" next pid : %d\n", next_pid );

  return next_pid;
}

int completed(struct processes *p, int np)
{
  int j;
  int flag = 1;

  for(j = 0; j < np; j++){
        if(p[j].finished==0){
          flag=0;
          break;
        }
      }
  return flag;
}

int get_preemption_time(struct processes *p, int np, int next_pid, int clock)
{
  int preemption_time = 0;

  for(int j = 0; j < np; j++){

        if(p[j].arrival_time>clock && p[j].arrival_time < clock + p[next_pid].remaining_time){
          preemption_time = p[j].arrival_time ;
          break;
        }
      }
  return preemption_time;
}


void stcf(struct processes *p, int np)
{
  // Sort processes in order of arrival time.
  sort(p, np);

  for(int i = 0; i<np; i++){
    p[i].finished = 0;
    p[i].remaining_time = p[i].burst_time;
    p[i].response_time = -1;
  }

  int response_time = 0;  
  // clock
  int clock=0;
  clock += p[0].arrival_time;

  while(!completed(p, np)){

      printf(" current clock moment : %d\n", clock);
      int next_pid = next_process_stcf(p, np, clock);
      int preemption_time = get_preemption_time(p, np, next_pid, clock);
      if (preemption_time){

        if(p[next_pid].response_time == -1) p[next_pid].response_time = clock - p[next_pid].arrival_time;
        p[next_pid].remaining_time = p[next_pid].remaining_time - (preemption_time-clock);
        clock = preemption_time;
        continue;

      }
      else {
        if(p[next_pid].response_time == -1) p[next_pid].response_time = clock - p[next_pid].arrival_time;

        clock += p[next_pid].remaining_time;
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

