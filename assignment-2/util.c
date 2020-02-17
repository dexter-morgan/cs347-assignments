#include "sched.h"

void sort(struct processes *p ,int np)
{
  // Sorts processes by arrival time.
  struct processes t;
  for(int i = 0; i < np -1 ;i++){
    for(int j = 0 ;j < np -i-1; j++)
      {
	if(p[j+1].arrival_time < p[j].arrival_time)
	  {
	    t = p[j+1];
	    p[j+1] = p[j];
	    p[j] = t;
	  }
      }
  }
}


void sort_rem(struct processes *p ,int np){
  // Sorts processes by remaining time with highest remaining time first. 
  struct processes t;
  for(int i = 0; i < np -1 ;i++) 
    {
      for(int j = 0 ;j < np -i-1; j++)
	{
	  if(p[j+1].remaining_time < p[j].remaining_time)
	    {
	      t = p[j+1];
	      p[j+1] = p[j];
	      p[j] = t;
	    }
	}
    }
}
