/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Sat 16 Apr 2016
**
** This header file is for implementation of a queue based on linked list
**
*/


/* constants */
#define TRUE 1
#define FALSE 0



/* sturcts */
// struct for each item to be stored in a node
typedef struct {
	int arrival_time;
	int process_id; // process if of this process
	int mem_size;	// memory size
	int job_time;	// corresponds to remaining time
} Process;



// struct for each Node of a queue
typedef struct _node {
	Process process; // item to be stored
	struct _node * next; //pointing the next node
} Node;


// struct for queue
typedef struct {
	Node* front;
	Node* rear;
	int num; // the number of current nodes
} Queue;




/* prototypes */
// Create a queue and return the pointer
Queue* create_queue();

// enqueue a new Item
void enqueue(Queue* queue, Process process);

// dequeue the first item and return the Item
Process dequeue(Queue* queue);

// check if this queue is empty or not
// return TRUE or FALSE
int is_queue_empty(Queue* queue);

// free all the malloced memory for the queue 
void free_queue(Queue * queue);









