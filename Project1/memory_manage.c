/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Sat 16 Apr 2016
**
** This file includes funtions to implement the memory management system 
**
*/



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "memory_manage.h"



/* 
** create memory manager and return its pointer
*/
Mem_manage* create_mem_manage(int size){
	// make sure that the size is bigger than 0
	assert(size > 0);

	// initialise this memory manager
	Mem_manage* mem_manage = (Mem_manage*)malloc(sizeof(Mem_manage));

	mem_manage->size = size;
	mem_manage->num_procs = 0;
	mem_manage->num_holes = 1;
	mem_manage->occupied_mem = 0;

	// make a new node representing a hole
	Mem_node* node = (Mem_node*)malloc(sizeof(Mem_node));

	// initialise memory table to be stored in the new node
	Memory_table mem_table = {NOT_PROCESS, size, 0};

	// put the mem_table into the new node
	node->mem_table = mem_table;
	node->next = NULL;

	mem_manage->mem_list = node;

	// return created memory manager
	return mem_manage;

}


/* put a given process into memory
** make use of largest_longest_process() and remove_process() when swapping out
*/
void put_process(Mem_manage* mem_manage, int process_id, int size){

	// if this process is already in memory, return
	if(is_in_memory(mem_manage, process_id))
		return;

	// take the first Mem_node
	Mem_node* cur = mem_manage->mem_list;
	// boolean value to check if a hole large enough has been found
	int is_there_hole = FALSE; 

	// id of process to be swapped out
	int id_removed;

	// search for a hole
	while( TRUE ){
		while(cur){
			// if we find a hole large enough, break the inner loop
			if(cur->mem_table.process_id == NOT_PROCESS &&
				cur->mem_table.mem_size >= size){
				is_there_hole = TRUE;
				break;
			}

			// investigate next node
			cur = cur->next;
		}

		// if a hole required found, break the outer loop
		if(is_there_hole){
			break;
		} else {
			// swap out the largest and longest process from memory
			id_removed = largest_longest_process(mem_manage);
			remove_process(mem_manage, id_removed);

			// re-iterate all nodes in memory to find out a hole large enough
			cur = mem_manage->mem_list;
		}

	}


	// put the process into the hole found
	cur->mem_table.process_id = process_id;
	cur->mem_table.residence_time = 0;


	// need to divide this node? 
	// (the hole found is bigger than necessary memory size?)
	if(cur->mem_table.mem_size  > size){
		Mem_node* new = (Mem_node*)malloc(sizeof(Mem_node));

		//initialise new node to represent a hole
		new->mem_table.process_id = NOT_PROCESS;
		new->mem_table.mem_size = cur->mem_table.mem_size - size;
		new->mem_table.residence_time = 0;
		new->next = cur->next;
		cur->next = new;

		// update the size of the current node
		cur->mem_table.mem_size = size;

		//update mem_manage
		mem_manage->num_procs++;

	// if the size of the hole is the same as the size required
	} else {
		mem_manage->num_procs++;
		mem_manage->num_holes--;
	}

	// update mem_manage
	mem_manage->occupied_mem += size;
}


/* 
** remove a given process from memory
*/
void remove_process(Mem_manage* mem_manage, int process_id){

	Mem_node* prev = NULL;
	Mem_node* cur = mem_manage->mem_list; 

	// iterate all the nodes until finding the right one 
	while (cur){
		// if we find the one to be removed
		if (cur->mem_table.process_id == process_id) {

			// make this node a hole
			cur->mem_table.process_id = NOT_PROCESS;
			cur->mem_table.residence_time = 0;

			//update mem_manager
			mem_manage->num_procs--;
			mem_manage->num_holes++;
			mem_manage->occupied_mem -= cur -> mem_table.mem_size;


			// deal with combining holes issue
			// if this is the first node of the list
			if ( prev == NULL ){
				// if the next node is a hole, combine
				if( cur->next != NULL &&
					cur->next->mem_table.process_id == NOT_PROCESS ){
					cur->mem_table.mem_size +=  cur->next->mem_table.mem_size;
					Mem_node* temp = cur->next;
					cur->next = cur->next->next;
					free(temp);

					//update mem_manager
					mem_manage->num_holes--;
				}

			// if this is the last node	
			} else if (cur->next == NULL) {
				// if the previous node is a hole, combine
				if (prev->mem_table.process_id == NOT_PROCESS){
					prev->mem_table.mem_size += cur->mem_table.mem_size;
					prev->next = NULL;
					free(cur);

					//update mem_manager
					mem_manage->num_holes--;
				}

			// the other locations
			} else {

				// if the both sides nodes are holes, combine the three
				if(prev->mem_table.process_id == NOT_PROCESS &&
					cur->next->mem_table.process_id == NOT_PROCESS){

					// increase mem_size of prev node 
					prev->mem_table.mem_size += cur->mem_table.mem_size;
					prev->mem_table.mem_size += cur->next->mem_table.mem_size;

					prev->next = cur->next->next;

					// free allocated memory
					free(cur->next);
					free(cur);

					//update mem_manager
					mem_manage->num_holes = mem_manage->num_holes - 2;

				// if the previous node is a hole, combine
				} else if (prev->mem_table.process_id == NOT_PROCESS){
					prev->mem_table.mem_size += cur->mem_table.mem_size;
					prev->next = cur->next;
					free(cur);

					//update mem_manager
					mem_manage->num_holes--;

				// if the next node is a hole, combine
				} else if (cur->next->mem_table.process_id == NOT_PROCESS){
					cur->mem_table.mem_size += cur->next->mem_table.mem_size;
					Mem_node* temp = cur->next;
					cur->next = cur->next->next;
					free(temp);

					//update mem_manager
					mem_manage->num_holes--;

				}
			}

			return;
		}

		// investigate the next node
		prev = cur;
		cur = cur->next;
	}

}


/* return TRUE if the given process is in memory
** otherwise FALSE
*/
int is_in_memory(Mem_manage* mem_manage, int process_id){
	// get the first node
	Mem_node* cur = mem_manage->mem_list;

	// iterate all the nodes in the list
	while ( cur ){
		if ( cur -> mem_table.process_id == process_id ){
			return TRUE;
		}
		cur = cur -> next;
	}
	return FALSE;
}


/*
** return id of swappable process
*/
int largest_longest_process(Mem_manage* mem_manage){
	assert(mem_manage->num_procs > 0);

	int required_proc_id = -1;
	int max_size = 0;
	int max_time = 0;

	// take the first node
	Mem_node* cur = mem_manage->mem_list;


	while( cur ){
		if ( cur->mem_table.process_id != NOT_PROCESS ){
			// if the memory size of current node is bigger than max_size
			if ( cur->mem_table.mem_size > max_size ) {
				required_proc_id = cur->mem_table.process_id;
				max_size = cur->mem_table.mem_size;
				max_time = cur->mem_table.residence_time;
				// if size is the same but it has longer residence time
			} else if ( cur->mem_table.mem_size == max_size && 
				cur->mem_table.residence_time > max_time ){
				required_proc_id = cur->mem_table.process_id;
				max_time = cur->mem_table.residence_time;
			}
		}

		cur = cur->next;
	}

	// return the largest and longest process id
	return required_proc_id;
}



/*
** increase the residence time of all processes in memory by 1
*/
void update_residence_time(Mem_manage* mem_manage){
	// get the first node
	Mem_node* cur = mem_manage->mem_list;

	// iterate all the nodes in the list
	while ( cur ){
		if ( cur -> mem_table.process_id != NOT_PROCESS ){
			cur -> mem_table.residence_time++;
		}
		cur = cur -> next;
	}

}



/*
** free all the malloced memory for the memory manager
*/
void free_mem_manage(Mem_manage* mem_manage){
	// make sure 'mem_manage' is not pointing NULL
	assert(mem_manage);
	Mem_node* cur = mem_manage->mem_list;

	// free every node in the list
	while (cur) {
		Mem_node* temp = cur;
		cur = cur->next;
		free(temp);
	}

	// free the mem_manage itself
	free(mem_manage);

}


