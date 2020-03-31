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
	int hare_race_finished;
	int tortoise_race_finished;
	int repositioning_done;
	int god_waiting_for_lock;
	int hare_waking_time;
	double time, hare_finish_time, tortoise_finish_time;
	int tortoise_distance, hare_distance;
	struct lock hare_tortoise_mutex;
	struct condition repositioning_cv;
	int current_repositioning_counter;

};

void* Turtle(struct race *race);
void* Hare(struct race *race);
void* God(struct race *race);
void* Report(struct race *race);

char init(struct race *race)
{
	
	printf("starting \n");
	race->hare_race_finished = 0;
	race->tortoise_race_finished = 0;

	race->time = 0;
	race->hare_waking_time = 0;
	race->tortoise_distance = 0;
	race->hare_distance = 0;
	race->repositioning_done=0;
	race->god_waiting_for_lock=0;
	race->current_repositioning_counter = 0;// to address the fact that no elements in repositiong array

	lock_init(&race->hare_tortoise_mutex);

	cond_init(&race->repositioning_cv);

	pthread_t t, h, g, r;
	pthread_create(&t, NULL, Turtle, race);
	pthread_create(&h, NULL, Hare, race);
	pthread_create(&g, NULL, God, race);
	pthread_create(&r, NULL, Report, race);
	printf("after creates\n");

	pthread_join(t, NULL);
	pthread_join(h, NULL);
	pthread_join(g, NULL);
	pthread_join(r, NULL);

	printf("hare total time : %f, tortoise total time: %f \n", race->hare_finish_time, race->tortoise_finish_time);
	
	if(race->hare_finish_time < race->tortoise_finish_time) return 'H';
	if(race->hare_finish_time > race->tortoise_finish_time) return 'T';
	return 'B';
}

void check_repositioning (struct race *race)
{   
    // check if the time for repositioning has come 
	if(race->time == race->reposition[race->current_repositioning_counter].time && race->repositioning_count>0){
		printf("time to wake up GOD! repositioning time : %f, player : %c \n", race->time, race->reposition[race->current_repositioning_counter].player);
		cond_signal(&race->repositioning_cv, &race->hare_tortoise_mutex);
		race->god_waiting_for_lock=1;
        // after waking up the god, wait till god is done repositioning. only continue after god is done
        while (!race->repositioning_done){
            printf("waiting for repositioning to finish \n");
            cond_wait(&race->repositioning_cv, &race->hare_tortoise_mutex);
        }
        printf("repositioning finished \n");
        race->repositioning_done=0;
		race->god_waiting_for_lock=0;
	}
} 

void* Turtle(struct race *race)
{
	
	printf("inside tortoise\n");

	while(race->tortoise_distance < race->finish_distance){
        
		lock_acquire(&race->hare_tortoise_mutex);
		// to make sure that god acquires lock when it is signaled. No player should progress when god is working
		if (race->god_waiting_for_lock){
			lock_release(&race->hare_tortoise_mutex);
			continue;
		}

        if(race->tortoise_distance + race->tortoise_speed < race->finish_distance){
            race->tortoise_distance += race->tortoise_speed * 1;
        } 
        else{
            float rem_time = (race->finish_distance - race->tortoise_distance) / (float)(race->tortoise_speed);
            race->tortoise_finish_time = race->time + rem_time;
            race->tortoise_distance = race->finish_distance;
			race->tortoise_race_finished = 1;
        }

		race->time+=1;
        check_repositioning(race);

		lock_release(&race->hare_tortoise_mutex);
		sleep_ms(1);
	}
	
	printf("tortoise completed race. exiting ... \n");
	pthread_exit(NULL);
	
}

void* Hare(struct race *race)
{
	printf("inside hare \n");

    // run the thread until hare reach finish line.
	while(race->hare_distance < race->finish_distance){

		lock_acquire(&race->hare_tortoise_mutex);
		if (race->god_waiting_for_lock){
			lock_release(&race->hare_tortoise_mutex);
			continue;
		}

        // update hare distance if not sleeping
        if (race->time >= race->hare_waking_time){
            if(race->hare_distance + race->hare_speed < race->finish_distance){
                race->hare_distance += race->hare_speed * 1;
            } 
            else{
                float rem_time = (race->finish_distance - race->hare_distance) / (float)(race->hare_speed);
                race->hare_finish_time = race->time + rem_time;
                race->hare_distance = race->finish_distance;
                race->hare_race_finished = 1;
            }
        }
        // hare goes to sleep if enough distance ahead of tortoise
        if(race->hare_distance - race->tortoise_distance > race->hare_turtle_distance_for_sleep){
			race->hare_waking_time = race->time + race->hare_sleep_time;
            printf("hare going for sleep. hare waking time : %d, distance : %d \n", race->hare_waking_time, race->hare_distance);
		}
        
		// initially tortoise increments the clock. If tortoise have finished the race, hare will update the clock
		if (race->tortoise_race_finished ==1)
			race->time+=1;
		
		check_repositioning(race);

		lock_release(&race->hare_tortoise_mutex);
		sleep_ms(1);
		
	}

	printf(" hare completed race. exiting ... \n");
	pthread_exit(NULL);
}

void* God(struct race *race)
{
	
	printf("inside god \n");

    // run until a repositioning remains
	while(race->current_repositioning_counter < race->repositioning_count){

		lock_acquire(&race->hare_tortoise_mutex);
        printf("lock acquired by god \n");

        // wait until the time of repositioning
		while(race->time < race->reposition[race->current_repositioning_counter].time){
            printf("God starting to wait for repositioning count : %d \n", race->current_repositioning_counter);
			cond_wait(&race->repositioning_cv, &race->hare_tortoise_mutex);
		}

        // if someone already finished a race, no need to continue. 
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
        // signal the thread waiting for the repositioning to finish
        cond_signal(&race->repositioning_cv, &race->hare_tortoise_mutex);
        lock_release(&race->hare_tortoise_mutex);
	}
    printf("GOD's job done. exiting ... \n");
	pthread_exit(NULL);
}

void* Report(struct race *race)
{
	printf("inside report \n");
    // no need to acquire lock for printing, as not changing any variable, also the resulting action doesnt change 
    // anyway based on the values. Also, if wait for acquiring locks, will not be able to print periodically
	while (race->hare_distance < race->finish_distance || race->tortoise_distance < race->finish_distance){

		// lock_acquire(&race->hare_tortoise_mutex);
		printf("T - %d; H - %d, time - %f \n",race->tortoise_distance, race->hare_distance, race->time);
		// lock_release(&race->hare_tortoise_mutex);

		sleep_ms(race->printing_delay);
	}
	pthread_exit(NULL);
}

