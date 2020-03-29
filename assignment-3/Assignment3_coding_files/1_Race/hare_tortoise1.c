#include "cs_thread.h"
#include <pthread.h>

int sleep_ms(int milliseconds)
{
    struct timespec ts;
    int res;

    if (milliseconds < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}


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
	int race_finished;
	int hare_race_finished;
    int hare_sleeping;
    int hare_waking_time;
    int repositioning_done;
	int tortoise_race_finished;
	double tortoise_time, hare_time, common_time;
	int tortoise_distance, hare_distance;
	struct lock tortoise_mutex, hare_mutex, reporter_mutex, hare_tortoise_mutex;
	struct condition repositioning_cv, repositioning_completed;
	int current_repositioning_counter;


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
	printf("starting \n");
	race->race_finished = 0;
	race->hare_race_finished = 0;
	race->tortoise_race_finished = 0;

    race->repositioning_done=0;
    race->hare_waking_time=0;
	race->tortoise_time = 0;
	race->hare_time = 0;
    race->common_time = 0;
	race->tortoise_distance = 0;
	race->hare_distance = 0;
	race->current_repositioning_counter = 0;// to address the fact that no elements in repositiong array

	lock_init(&race->tortoise_mutex);
	lock_init(&race->hare_mutex);
	lock_init(&race->reporter_mutex);
    lock_init(&race->hare_tortoise_mutex);

	cond_init(&race->repositioning_cv);
    cond_init(&race->repositioning_completed);

	/*void ** char_ptr_t, char_ptr_h, char_ptr_g, char_ptr_r;
	char char_t, char_g, char_h, char_r;*/

	pthread_t t, h, g, r;
	pthread_create(&t, NULL, Turtle, race);
	pthread_create(&h, NULL, Hare, race);
	pthread_create(&g, NULL, God, race);
	pthread_create(&r, NULL, Report, race);
	printf("after creates\n");
	pthread_join(t, NULL);
	printf("after t joins\n");
	
	pthread_join(h, NULL);
	printf("after h joins\n");
	pthread_join(g, NULL);
	printf("after g joins\n");
	pthread_join(r, NULL);
	printf("after r joins\n");
	printf("hare total time : %f, tortoise total time: %f \n", race->hare_time, race->tortoise_time);
	//char_h = char; ///who wins here, int or char to return
	if(race->hare_time < race->tortoise_time) return 'H';
	if(race->hare_time > race->tortoise_time) return 'T';
	return 'B';
}

void check_repositioning (struct race *race)
{
	if(race->common_time == race->reposition[race->current_repositioning_counter].time){
		printf("time to wake up GOD! repositioning time : %f, player : %c \n", race->common_time, race->reposition[race->current_repositioning_counter].player);
		cond_signal(&race->repositioning_cv, &race->hare_tortoise_mutex);
        while (!race->repositioning_done){
            printf("waiting for repositioning to finish \n");
            cond_wait(&race->repositioning_cv, &race->hare_tortoise_mutex);
        }
        printf("repositioning finished \n");
        race->repositioning_done=0;
	}
}     

void* Turtle(struct race *race)
{
	
	printf("inside tortoise \n");
	return NULL;
	while(race->tortoise_distance < race->finish_distance){
		lock_acquire(&race->tortoise_mutex);
		if(race->tortoise_distance + race->tortoise_speed < race->finish_distance){
			sleep_ms(1);
			race->tortoise_distance += race->tortoise_speed * 1;
			race->tortoise_time += 1;
			// check_repositioning_tortoise(race);//check repositioning function tbd
		} else{
			float rem_time = (race->finish_distance - race->tortoise_distance) / (float)(race->tortoise_speed);
			sleep_ms(rem_time);
			race->tortoise_time += rem_time;
			race->tortoise_distance = race->finish_distance;
			race->tortoise_race_finished = 1;
			if (race->hare_race_finished==1)
				race->race_finished=1;
			// check_repositioning_tortoise(race);
			break;
		}
		lock_release(&race->tortoise_mutex);
		// sleep_ms(10);
	}
	
	lock_release(&race->tortoise_mutex);
	printf("tortoise completed race. exiting ... \n");
	pthread_exit(NULL);
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
	printf("inside hare \n");
	// return NULL;
	while(race->hare_distance < race->finish_distance || race->tortoise_distance < race->finish_distance){
		lock_acquire(&race->hare_tortoise_mutex);
		
        if (!race->tortoise_race_finished){

            if(race->tortoise_distance + race->tortoise_speed < race->finish_distance){
                race->tortoise_distance += race->tortoise_speed * 1;
            } 
            else{
                float rem_time = (race->finish_distance - race->tortoise_distance) / (float)(race->tortoise_speed);
                race->tortoise_time = race->common_time + rem_time;
                race->tortoise_distance = race->finish_distance;
                race->tortoise_race_finished = 1;
                // if (race->hare_race_finished==1)
                // race->race_finished=1;
                // break;
            }
        }

        if (!race->hare_race_finished && race->common_time >= race->hare_waking_time){
            if(race->hare_distance + race->hare_speed < race->finish_distance){
                race->hare_distance += race->hare_speed * 1;
            } 
            else{
                float rem_time = (race->finish_distance - race->hare_distance) / (float)(race->hare_speed);
                race->hare_time = race->common_time + rem_time;
                race->hare_distance = race->finish_distance;
                race->hare_race_finished = 1;
                // if (race->tortoise_race_finished==1)
                // race->race_finished=1;
                // break;
            }
        }

        if(race->hare_distance - race->tortoise_distance > race->hare_turtle_distance_for_sleep){
			race->hare_waking_time = race->common_time + race->hare_sleep_time;
            printf("hare going for sleep. hare waking time : %d, distance : %d \n", race->hare_waking_time, race->hare_distance);
		}
        race->common_time+=1;
        sleep_ms(1);
        check_repositioning(race);
        
		lock_release(&race->hare_tortoise_mutex);
		
	}
    // lock_release(&race->hare_tortoise_mutex);
	printf(" completed race. exiting ... \n");
	pthread_exit(NULL);
	return NULL;
}

void* God(struct race *race)
{

	printf("inside god \n");
	// return NULL;

	if((race->repositioning_count == 0)){
		pthread_exit(NULL);
	}
	while(race->current_repositioning_counter < race->repositioning_count){
		lock_acquire(&race->hare_tortoise_mutex);
        printf("lock acquired by god \n");
		while(race->common_time < race->reposition[race->current_repositioning_counter].time){
            printf("God starting to wait for repositioning count : %d \n", race->current_repositioning_counter);
			cond_wait(&race->repositioning_cv, &race->hare_tortoise_mutex);
		}

        if (race->tortoise_race_finished || race->hare_race_finished){
            lock_release(&race->hare_tortoise_mutex);
            pthread_exit(NULL);
        }
            

        printf("Before repositioning - T : distance - %d; H : distance - %d \n",race->tortoise_distance, race->hare_distance );
        int final_distance = 0;
		if(race->reposition[race->current_repositioning_counter].player == 'T') {
            final_distance = race->tortoise_distance + race->reposition[race->current_repositioning_counter].distance;
            if (final_distance > race->finish_distance){
                race->tortoise_distance = race->finish_distance;
                race->tortoise_race_finished = 1;
            }
            else if (final_distance < 0){
                race->tortoise_distance = 0;
            }              
            else {
                race->tortoise_distance = final_distance;
            }
                
		}
        if(race->reposition[race->current_repositioning_counter].player == 'H') {
            final_distance = race->hare_distance + race->reposition[race->current_repositioning_counter].distance;
            if (final_distance > race->finish_distance){
                race->hare_distance = race->finish_distance;
                race->hare_race_finished = 1;
            }
            else if (final_distance < 0){
                race->hare_distance = 0;
            }     
            else {
                race->hare_distance = final_distance;
            }         
		}
        race->repositioning_done=1;
		race->current_repositioning_counter++;
        printf("After repositioning - T : distance - %d; H : distance - %d \n",race->tortoise_distance, race->hare_distance );
        cond_signal(&race->repositioning_cv, &race->hare_tortoise_mutex);
        lock_release(&race->hare_tortoise_mutex);
	}
    printf("GOD's job done. exiting ... \n");
	pthread_exit(NULL);
	return NULL;
}

void* Report(struct race *race)
{
	printf("inside report \n");
	while (race->hare_distance < race->finish_distance || race->tortoise_distance < race->finish_distance){
        // lock_acquire(&race->hare_tortoise_mutex);
		printf("T - %d; H - %d, time - %f \n",race->tortoise_distance, race->hare_distance, race->common_time );
        // lock_release(&race->hare_tortoise_mutex);
		sleep_ms(race->printing_delay*1);
	}
	pthread_exit(NULL);
}

