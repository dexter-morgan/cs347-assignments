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
    int hare_waking_time;
    int repositioning_done;
	int tortoise_race_finished;
	double tortoise_time, hare_time, common_time;
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

    race->repositioning_done=0;
    race->hare_waking_time=0;
	race->tortoise_time = 0;
	race->hare_time = 0;
    race->common_time = 0;
	race->tortoise_distance = 0;
	race->hare_distance = 0;
	race->current_repositioning_counter = 0;// to address the fact that no elements in repositiong array

    lock_init(&race->hare_tortoise_mutex);
	cond_init(&race->repositioning_cv);

	pthread_t t, h, g, r;
	pthread_create(&t, NULL, Turtle, race);
	pthread_create(&h, NULL, Hare, race);
	pthread_create(&g, NULL, God, race);
	pthread_create(&r, NULL, Report, race);

	pthread_join(t, NULL);
	pthread_join(h, NULL);
	pthread_join(g, NULL);
	pthread_join(r, NULL);
	printf("hare total time : %f, tortoise total time: %f \n", race->hare_time, race->tortoise_time);
	//char_h = char; ///who wins here, int or char to return
	if(race->hare_time < race->tortoise_time) return 'H';
	if(race->hare_time > race->tortoise_time) return 'T';
	return 'B';
}

void check_repositioning (struct race *race)
{   
    // check if the time for repositioning has come 
	if(race->common_time == race->reposition[race->current_repositioning_counter].time){
		printf("time to wake up GOD! repositioning time : %f, player : %c \n", race->common_time, race->reposition[race->current_repositioning_counter].player);
		cond_signal(&race->repositioning_cv, &race->hare_tortoise_mutex);
        // after waking up the god, wait till god is done repositioning. only continue after god is done
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
  
}

void* Hare(struct race *race)
{

	printf("inside hare \n");

    // run the thread until both reach finish line. finally compare the time taken to decide the winner
	while(race->hare_distance < race->finish_distance || race->tortoise_distance < race->finish_distance){
		lock_acquire(&race->hare_tortoise_mutex);
		
        // if tortoise reached finish line, no need to update anymore
        if (!race->tortoise_race_finished){

            if(race->tortoise_distance + race->tortoise_speed < race->finish_distance){
                race->tortoise_distance += race->tortoise_speed * 1;
            } 
            else{
                float rem_time = (race->finish_distance - race->tortoise_distance) / (float)(race->tortoise_speed);
                race->tortoise_time = race->common_time + rem_time;
                race->tortoise_distance = race->finish_distance;
                race->tortoise_race_finished = 1;
            }
        }
        // update hare distance if havent reached finish line already and is not sleeping
        if (!race->hare_race_finished && race->common_time >= race->hare_waking_time){
            if(race->hare_distance + race->hare_speed < race->finish_distance){
                race->hare_distance += race->hare_speed * 1;
            } 
            else{
                float rem_time = (race->finish_distance - race->hare_distance) / (float)(race->hare_speed);
                race->hare_time = race->common_time + rem_time;
                race->hare_distance = race->finish_distance;
                race->hare_race_finished = 1;
            }
        }
        // hare goes to sleep if enough distance ahead of tortoise
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

    // run until a repositioning remains
	while(race->current_repositioning_counter < race->repositioning_count){
		lock_acquire(&race->hare_tortoise_mutex);
        printf("lock acquired by god \n");

        // wait until the time of repositioning
		while(race->common_time < race->reposition[race->current_repositioning_counter].time){
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
        // signal the hare thread waiting for the repositioning to finish
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
    // no need to acquire lock for printing, as not changing any variable, also the resulting action doesnt change 
    // anyway. Also, if want for acquiring locks, will not be able to print periodically
	while (race->hare_distance < race->finish_distance || race->tortoise_distance < race->finish_distance){
		printf("T - %d; H - %d, time - %f \n",race->tortoise_distance, race->hare_distance, race->common_time );
		sleep_ms(race->printing_delay*1);
	}
	pthread_exit(NULL);
}

