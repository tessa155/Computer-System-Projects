/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Sat 16 Apr 2016
**
** This header file is for implementation of memory management system.
** The basic data sturcture is a linked list and
** both the spaces occupied by processes and the empty spaces are
** represented by the same struct which is Mem_node and each Mem_node
** has Memory_table in it. 
** If the node is representing a hole, the value of process_id in its
** Memory_table is -1 (NOT_PROCESS) 
**
** If any more than two consecutive holes are detected 
** as the Mem_manage is being filled up by processes, the holes are going to
** be combined together.
**
*/


/* constants */
#define TRUE 1
#define FALSE 0
#define NOT_PROCESS -1


/* structs */
// struct for each item to be stored in a node
typedef struct {
	int process_id; // process if of this process
	int mem_size;
	int residence_time; // how long this process has been on memory
} Memory_table;


// struct for each Node of this linked list
typedef struct _mem_node {
	Memory_table mem_table; // item to be stored
	struct _mem_node * next;
} Mem_node;


// struct for linked list to manage memory
typedef struct {
	int size;			// the whole memory size
	int num_procs; 		// the number of processes in memory
	int num_holes; 		// the number of holes
	int occupied_mem; 	// occupied memory
	Mem_node* mem_list; // linked list of processes and holes in memory
} Mem_manage;



/* prototypes */
// creage memory manager and return its pointer
Mem_manage* create_mem_manage(int size);

// put a given process into memory
// make use of largest_longest_process() and remove_process() when swapping out
void put_process(Mem_manage* mem_manage, int process_id, int size);

// remove a given process from memory
void remove_process(Mem_manage* mem_manage, int process_id);

// return process id of swappable process
int largest_longest_process(Mem_manage* mem_manage);

// return TRUE if the given process is in memory
// otherwise FALSE
int is_in_memory(Mem_manage* mem_manage, int process_id);

// increase the residence time of all processes in memory by 1
void update_residence_time(Mem_manage* mem_manage);

// free all the malloced memory for the memory list
void free_mem_manage(Mem_manage* mem_manage);




