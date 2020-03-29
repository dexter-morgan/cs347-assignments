#include "cs_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {

	int h_atom_count; 
	struct lock h_count_lock, reaction_process;
	struct condition h_enough, reaction_completed;
};

void
reaction_init(struct reaction *reaction)
{	

	lock_init(&reaction->h_count_lock);
	lock_init(&reaction->reaction_process);

	cond_init(&reaction->h_enough);
	cond_init(&reaction->reaction_completed);

	reaction->h_atom_count=0;
	
}

void
reaction_h(struct reaction *reaction)
{	

	lock_acquire(&reaction->h_count_lock);
	reaction->h_atom_count++;

	if (reaction->h_atom_count>=2){
		cond_signal(&reaction->h_enough, &reaction->h_count_lock);
	}
	printf("h atom starting to wait. H atom id : %ld \n", pthread_self());
	cond_wait(&reaction->reaction_completed, &reaction->h_count_lock);
	
	reaction->h_atom_count--;
	lock_release(&reaction->h_count_lock);
	return;
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->h_count_lock);

	while (reaction->h_atom_count<2){
		printf("o atom starting to wait. O atom id : %ld.  not enough atoms \n", pthread_self());
		cond_wait(&reaction->h_enough, &reaction->h_count_lock);
	}
	

	lock_acquire(&reaction->reaction_process);
	printf("making water \n");
	make_water();
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	printf("completed signalling h atoms \n");
	lock_release(&reaction->reaction_process);
	lock_release(&reaction->h_count_lock);
	return;
}