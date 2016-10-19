/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Fri 27 May 2016
**
** This script is the head file for server.c 
** 
*/

#define CHANCES 	10
#define TRUE 		1
#define FALSE 		0
#define AVAILABLE 	-1
#define FULL 		-1
#define BUFFER_SIZE 256


/* struct for argument to each thread*/
typedef struct {
	char* secret_code; 
	int fd;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int newsockfd;
} Thread_struct;


/* prototypes */
void generate_secret_code(char* secret_code);
int check_code(char* code);
void generate_feedback(int* a, int* b, char* secret_code, char* code);
void record_time(int fd);
void record_message(int fd, struct sockaddr_in addr, int id, char* msg, char* arg);
void *serve_client(void *param);  
void initialise_thread_struct_array(Thread_struct* inform_arr);
int available_spot(Thread_struct* inform_arr);
void signal_handler(int signal);




