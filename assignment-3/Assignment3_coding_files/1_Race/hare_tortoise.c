#include "cs_thread.h"

struct Repositioning {
	char player; 		// T for turtle and H for hare
	int time; 		// At what time god interrupt's
	int distance;		// How much does god move any of the player. 
							// distance can be negetive or positive.
							// using this distance if any of players position is less than zero then bring him to start line.
							// If more than finish_distance then make him win.
							// If time is after completion of game than you can ignore it as we will already have winner.
};

struct race {
	/*
		You might want to store here locks, conditional variables that will be used.
		Feel free to add here other variables like distance covered by hare/tortoise, flags, etc.
		Some arguments recieved from camand line are already added.
	*/

	//	Don't change this variables.
	//	speeds are unit distance per milliseconds.
	// 	time is in seconds.
	int printing_delay;
	int tortoise_speed;					// speed of Turtle
	int hare_speed;						// speed of hare
	int hare_sleep_time; 				// how much time does hare sleep (in case he decides to sleep)
	int hare_turtle_distance_for_sleep; // minimum distance between hare and turtle when turtle sleeps
	int finish_distance;				// Distance between start and finish line
	struct Repositioning* reposition;	// pointer to array containing gods decision
	int repositioning_count;			// number of elements in array of repositioning structure
	
	//	Add your custom variables here.
};
void* Turtle(struct race *race);
void* Hare(struct race *race);
void* God(struct race *race);
void* Report(struct race *race);

char init(struct race *race)
{
	/*
	use : Runs on main thread
	arguments :
		struct race *race : collection of data that will be used across threads.
	reqirements:
		In this function you should create 4 threads and start given function in thier respective threads.
		Note : This thread should exit only when all thread have finished execution.
		Just under is example pseudocode. 
		Fell free to use your own implementation. 
		You will get marks as long as all conditions are handled.
	Marks alloted : 10
	Pseudocode:
	thread create {
		Tutle()
		Hare()
		God()
		Report()
	} 
	loop or wait until somebody wins;
	return T/H/B for winner (Turtle/Hare/Both end at same time)
	*/
	return '-'; 
}

void* Turtle(struct race *race)
{
	/*
	use : Runs on turtle thread
	arguments :
		struct race *race : collection of data that will be used across threads.
	reqirements:
		keep moving in order leg1 leg2 leg3 leg4 till race is not finished
		Just under is example pseudocode. 
		Fell free to use your own implementation. 
		You will get marks as long as all conditions are handled.
	Marks alloted : 10
	Pseudocode:
	while race not finished :
		// 	Here moveleg can be replaced by simple sleep  for time it takes to comple one unit distance.
		//	And then increase	 the distaance covered by 1.
		//	There is one example sleep_ms function provided. Feel free to use any other implementation if you wish.
		moveleg(1)
		moveleg(2)
		moveleg(3)
		moveleg(4)
	*/
	return NULL;
  
}

void* Hare(struct race *race)
{
	/*
	use : Runs on hare thread
	arguments :
		struct race *race : collection of data that will be used across threads.
	reqirements:
		If turtle is far behind sleep else run really fast for some time.
		Just under is example pseudocode. 
		Fell free to use your own implementation. 
		You will get marks as long as all conditions are handled.
	Marks alloted : 10
	Pseudocode:
	while race not over:
		while Turtle is far behind:
			Sleep(for_a_while);
		RunLikeCrazy_A_bit();
	*/
	return NULL;
}

void* God(struct race *race)
{
	/*
	use : Runs on god thread
	arguments :
		struct race *race : collection of data that will be used across threads.
	reqirements:
		repositioning Turtle or Hare depending on content of "Repositioning" structure
		Just under is example pseudocode. 
		Fell free to use your own implementation. 
		You will get marks as long as all conditions are handled.
	Marks alloted : 10
	Pseudocode:
		when time equals to Repositiong.time then move Repositioning.player by Repositioning.distance
	*/
	return NULL;
}

void* Report(struct race *race)
{
	/*
	use : Runs on report thread
	arguments :
		struct race *race : collection of data that will be used across threads.
	reqirements:
		Periodically print distance covered by hare and tortoise from start of the file.
		Just under is example pseudocode. 
		Fell free to use your own implementation. 
		You will get marks as long as all conditions are handled.
	Marks alloted : 10
	Pseudocode:
	while game not over:
		report positions of hare and turtle format "T : distance from start line; H : distance from start linessss"
	*/
	return NULL;
}

