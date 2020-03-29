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
		cond_signal(&reaction->h_enough, &reaction->h_count_lock);
	}
	// printf("h atom starting to wait. H atom id : %ld \n", pthread_self());
	cond_wait(&reaction->reaction_completed, &reaction->h_count_lock);
	
	reaction->h_atom_count--;
	// printf("H atom signing off. hcount : %d\n", reaction->h_atom_count);
	lock_acquire(&reaction->pending_returns_lock);
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
	while(reaction->pending_h_returns>0){
		cond_wait(&reaction->h_returned, &reaction->pending_returns_lock);
	}
	lock_acquire(&reaction->h_count_lock);

	while (reaction->h_atom_count<2){
		// printf("o atom starting to wait. O atom id : %ld.  not enough atoms \n", pthread_self());
		cond_wait(&reaction->h_enough, &reaction->h_count_lock);
	}
	// printf("inside o ready. hcount : %d\n", reaction->h_atom_count);
	printf("making water \n");
	make_water();
	reaction->pending_h_returns=2;
	lock_release(&reaction->pending_returns_lock);
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	cond_signal(&reaction->reaction_completed, &reaction->reaction_process);
	// printf("completed signalling h atoms \n");
	// printf("hcount : %d\n", reaction->h_atom_count);
	lock_release(&reaction->h_count_lock);
	lock_release(&reaction->reaction_process);
	 
	return;
}