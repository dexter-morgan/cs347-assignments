#include "cs_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {

	int h_atom_count; 
	int pending_h_returns;
	struct lock h_count_lock, reaction_process, pending_returns_lock;
	struct condition h_enough, reaction_completed, h_returned;
};

void
reaction_init(struct reaction *reaction)
{	

	lock_init(&reaction->h_count_lock);
	lock_init(&reaction->pending_returns_lock);
	lock_init(&reaction->reaction_process);

	cond_init(&reaction->h_enough);
	cond_init(&reaction->reaction_completed);
	cond_init(&reaction->h_returned);

	reaction->h_atom_count=0;
	reaction->pending_h_returns=0;
	
}

void
reaction_h(struct reaction *reaction)
{	

	lock_acquire(&reaction->h_count_lock);
	reaction->h_atom_count++;

	if (reaction->h_atom_count>=2){
		// if an o atom waiting for enough h atoms to accumulate, wake him up 
		cond_signal(&reaction->h_enough, &reaction->h_count_lock);
	}
	// wait until a reaction is completed. 
	cond_wait(&reaction->reaction_completed, &reaction->h_count_lock);
	// will reach here only if a reaction is completed.reduce h atom count. 
	reaction->h_atom_count--;
	lock_acquire(&reaction->pending_returns_lock);
	// make sure these 2 h atoms return. when both the h atoms involved in the reaction are returned, signal
	// the waiting o atom to proceed. this will make sure o atom doesnt see more h atoms than are available
	// for reaction
	reaction->pending_h_returns--;
	if (reaction->pending_h_returns==0){
		cond_signal(&reaction->h_returned, &reaction->pending_returns_lock);
	}
	lock_release(&reaction->pending_returns_lock);
	lock_release(&reaction->h_count_lock);
	return;
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->reaction_process);
	lock_acquire(&reaction->pending_returns_lock);
	// if h atoms from previous reaction are yet to return, wait for them to return. only then proceed. This will ensure
	// correct value of h count.
	while(reaction->pending_h_returns>0){
		cond_wait(&reaction->h_returned, &reaction->pending_returns_lock);
	}
	lock_acquire(&reaction->h_count_lock);

	while (reaction->h_atom_count<2){
		// if less than 2 atoms present, wait for enough h atoms to accumulate
		cond_wait(&reaction->h_enough, &reaction->h_count_lock);
	}
	// printf("making water \n");
	make_water();
	// set no of h atoms to return = 2. only allow next o atom to proceed after these 2 h atoms have returned
	reaction->pending_h_returns=2;
	lock_release(&reaction->pending_returns_lock);
	// signal the 2 h atoms used for the reaction.
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	lock_release(&reaction->h_count_lock);
	lock_release(&reaction->reaction_process);
	 
	return;
}