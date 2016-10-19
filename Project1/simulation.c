/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Sat 16 Apr 2016
**
** This script is for simlulation of scheduling and memory management
** of CPU. There are two different algorithms used which are
** first come first serve and multi leveled feedback queue
** 
** 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "memory_manage.h"


/* constants */
#define MAX_LEN 		100 	// MAX LENGTH of each line of input text file
#define FIRST_QUANTUM 	4;		// first quantum value for multi leveled queue
#define SEC_QUANTUM 	8;		// second quantum value
#define THIRD_QUANTUM 	12;		// third quantum value



/* prototypes */
void parse_input_file(Queue* proc_que, char *input);
void fcfs(Queue* proc_que, int size);
void multi(Queue* proc_que, int size);



/* Extern declarations: */
extern  int     optind;
extern  char    *optarg;



/*
** main function which simulate scheduling and memory management by calling
** appropriate functions
*/  
int main(int argc, char *argv[]) {
	// queue whcih contains all the processes in order of their arrival times 
	Queue* proc_que = create_queue();

	// f(input file), a(algorithm), or m(memory size)
	char flag; 
	// input text file
	char *input;
	// size of memory 
	int size; 
	// algorithm to be used (fcfs or multi)
	char *algo = "fcfs";	


	// parse the command line and fill input, size and algo variables
	while ((flag = getopt(argc, argv, "f:a:m:")) != EOF) {
		switch ( flag ) {
			case 'f':
				input = optarg;
 				break;

			case 'a':
				if(strcmp(optarg, "multi") == 0) 
					algo = optarg;

				else if(strcmp(optarg, "fcfs") != 0) 
				{
					// exit if optarg unknown
					fprintf(stderr, "Unknown %s\n ", optarg);
					exit(1);
				}
 				break;
					// should not get here	
			case 'm':
				// set the value of size (int value) based on optarg
				size = atoi(optarg);
				break;

			default:
				// do something
				break;
		} 
	}
	

	// parse the input text file and put all processes into proc_que
	parse_input_file(proc_que, input);


	// run simulation using the chosen algorithm
	if ( strcmp(algo, "fcfs") == 0 ) {
		fcfs(proc_que, size);
	} else {
		multi(proc_que, size);
	}

	// free allocated memory
	free_queue(proc_que);

	return 0;

}




/* 
** scheduling using first come first serve algorithm
*/
void fcfs(Queue* proc_que, int size){
	// counter variable
	int i = 0;
	// flag for checking if there is any process running now
	int is_running = FALSE;
	// process running currently
	Process cur_proc;

	// create memory manager
	Mem_manage* mem_manage = create_mem_manage(size);


	// counter loop for this simulation
	while(TRUE){

		// if there is a process running, decrease the job time by 1
		if (is_running){
			cur_proc.job_time--;

			//  if the current job finished
			if(cur_proc.job_time == 0){
				is_running = FALSE;
				// remove this process from memory
				remove_process(mem_manage, cur_proc.process_id);
			}

		}

		// run the next process in the queue if the current one has finished
		if ( !is_queue_empty(proc_que) &&
			proc_que->front->process.arrival_time <= i && !is_running){

			cur_proc = dequeue(proc_que);

			// put this process on the memory from disk
			put_process(mem_manage, cur_proc.process_id, cur_proc.mem_size);

			// set is_running true so that this process can run 
			is_running = TRUE;

			int mem_usage = (double)mem_manage->occupied_mem / mem_manage->size * 100;

			printf("time %d, %d running, numprocesses=%d, numholes=%d, memusage=%d%%\n",
			i, cur_proc.process_id, mem_manage->num_procs, mem_manage->num_holes, mem_usage);

		}

		// if there is no process in the queue and has finished the last job, break the loop
		if(is_queue_empty(proc_que) && cur_proc.job_time == 0)
			break;

		// increase counter by 1
		i++;

		// increase the residence time of all processes in memory by 1
		update_residence_time(mem_manage);

	}

	// simulation finished
	printf("time %d, simulation finished.\n", i);


	//free all allocated memory
	free_mem_manage(mem_manage);

}



/* 
** scheduling using multi leveled feedback queue
*/
void multi(Queue* proc_que, int size){
	int i = 0;
	int is_running = FALSE;
	Process cur_proc;
	int quantum = 0;
	// current queue level of the currently running process (1, 2 or 3)
	int cur_level = 0; 


	// create three queues to implement multi leveled scheduling
	Queue* proc_que_1 = create_queue();
	Queue* proc_que_2 = create_queue();
	Queue* proc_que_3 = create_queue();

	// create memory manager
	Mem_manage* mem_manage = create_mem_manage(size);


	// counter loop for this simulation
	while(TRUE){

		// if there is a process running, decrease both its job time and quantum by 1
		if (is_running){
			cur_proc.job_time--;
			quantum--;
			
			//  if the current job finished, remove it from memory
			if(cur_proc.job_time == 0){
				remove_process(mem_manage, cur_proc.process_id);

				// initialise flags
				is_running = FALSE;
				cur_level = 0;

			// if the given quantum has been consumed, put the processs into the next level queue
			} else if (quantum == 0) {

				//from first queue to second
				if (cur_level == 1){
					enqueue(proc_que_2, cur_proc);
				//from second to third or from third to thrid
				} else if (cur_level == 2 || cur_level == 3){
					enqueue(proc_que_3, cur_proc);
				} 
				is_running = FALSE;

			}			
		}

		// put processes into the first queue whose arrival time is equal to i
		while (proc_que->num > 0){
			if (proc_que->front->process.arrival_time == i){
				Process temp = dequeue(proc_que);
				enqueue(proc_que_1, temp);
			} else {
				break;
			}
	
		}

		// run the next process if there is one to run and the previous one has finished 
 		// check the first queue first and then second and third queue
		// set is_running, cur_proc, quantum, cur_level flags right
		if (!is_running) {
			if(!is_queue_empty(proc_que_1)){
				cur_proc = dequeue(proc_que_1);
				quantum = FIRST_QUANTUM;
				cur_level = 1;
			}else if(!is_queue_empty(proc_que_2)){
				cur_proc = dequeue(proc_que_2);
				quantum = SEC_QUANTUM;
				cur_level = 2;
			}else if(!is_queue_empty(proc_que_3)){
				cur_proc = dequeue(proc_que_3);
				quantum = THIRD_QUANTUM;
				cur_level = 3;
			}

			// if there is a process to run
			if(cur_level != 0){
				// put this process on memory
				put_process(mem_manage, cur_proc.process_id, cur_proc.mem_size);
				is_running = TRUE;

				int mem_usage = (double)mem_manage->occupied_mem / mem_manage->size * 100;

				printf("time %d, %d running, numprocesses=%d, numholes=%d, memusage=%d%%\n",
			i, cur_proc.process_id, mem_manage->num_procs, mem_manage->num_holes, mem_usage);

			}
			
		}

		// if there is no any process to run, break the loop
		if (is_queue_empty(proc_que) && cur_proc.job_time == 0)
			break;

		// increase counter by 1
		i++;

		// increase the residence time of all processes in memory by 1
		update_residence_time(mem_manage);

	}


	// simulation finished
	printf("time %d, simulation finished.\n", i);

	//free all allocated memory
	free_mem_manage(mem_manage);
	free_queue(proc_que_1);
	free_queue(proc_que_2);
	free_queue(proc_que_3);


}




/* parse the input file and build a queue which links 
** all the processes
*/
void parse_input_file(Queue* proc_que, char *input){

	// open the file stream
	FILE * fp = fopen(input, "rt");
	assert(fp);
	
	char str[MAX_LEN];
	Process temp;

	// parse each line of the input file and put it 
	// in the queue
	while (fgets(str, MAX_LEN, fp) != NULL) {
		sscanf(str, "%d %d %d %d", 
			&temp.arrival_time, &temp.process_id, &temp.mem_size, &temp.job_time);
		enqueue(proc_que, temp);
	}

	fclose(fp);
}




