/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Sat 16 Apr 2016
**
** This file includes funtions to implement a queue
** based on linked list
** 
** Attribution: 
**  Some of basic frame of this script was refered from the book,
**  Introduction to Data Structures Using C by Sung-Woo Yoon
** 
*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"


/* 
** Create a queue and return its pointer
*/
Queue* create_queue(){

	// allocate appropriate memory for Queue
	Queue* queue = (Queue *)malloc(sizeof(Queue));
	assert(queue);

	// fill the basic information
	queue->front = NULL;
	queue->rear = NULL;
	queue->num = 0;

	// return the Queue pointer
	return queue;

}

/*
** enqueue a new Process
*/
void enqueue(Queue* queue, Process process){
	// make sure 'queue' is not pointing NULL
	assert(queue);

	// malloc a new node
	Node* new_node = (Node *)malloc(sizeof(Node));
	assert(new_node);

	// put the process in the new node
	new_node->process = process;
	new_node->next = NULL;

	// check if the queue is empty
	if(is_queue_empty(queue)){
		// both front and rear are pointing to this new node
		queue->front = new_node;
		queue->rear = new_node;

	}else{
		// put this new node in the queue
		// and update 'next' of the previous rear node
		queue->rear->next = new_node;
		queue->rear = new_node;

	}
	// increase the number of nodes
	queue->num++;

}


/*
** dequeue the first process and return it
*/
Process dequeue(Queue* queue){
	// make sure 'queue' is not pointing NULL
	assert(queue);

	Node* del_node;
	Process del_process;
	
	// make sure the queue is not empty
	assert(!is_queue_empty(queue));

	// store the first process temporally and remove it from the queue 
	del_node = queue->front;
	del_process = del_node->process;
	queue->front = queue->front->next;

	// decrease the number of nodes in the queue
	queue->num--;

	// if the number of the queue is zero now,
	// make the rear point to NULL;
	if(queue->num == 0)
		queue->rear = NULL;

	// free memory
	free(del_node);

	// return the deleted process
	return del_process;

}



/* check if this queue is empty or not
** return TRUE or FALSE
*/
int is_queue_empty(Queue* queue){
	// make sure 'queue' is not pointing NULL
	assert(queue);

	if(queue->num == 0)
		return TRUE;
	else
		return FALSE;

}



/*
** free all the malloced memory for the queue 
*/
void free_queue(Queue * queue){
	// make sure 'queue' is not pointing NULL
	assert(queue);

	// free every node in the queue by dequeuing
	while(!is_queue_empty(queue)){
		dequeue(queue);
	}

	// free the queue itself
	free(queue);

}


