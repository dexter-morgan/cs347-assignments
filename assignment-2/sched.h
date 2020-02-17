#include<stdio.h>
#include<stdlib.h>

struct processes
{
	int arrival_time;
	int burst_time;
	int process_id;
	int turnaround_time;
	int waiting_time;
	int remaining_time;
	int leave_time;
	int response_time;
	int finished;
	int priority;
};



void sort(struct processes *p ,int np);
void sort_rem(struct processes *p ,int np);

void fcfs(struct processes *p, int np);
void sjf(struct processes *p, int np);
void stcf(struct processes *p, int np);
void lottery(struct processes *p, int np);