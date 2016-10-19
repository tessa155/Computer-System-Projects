/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Fri 27 May 2016
**
** This script is for implementation of a simple game server which communicates with
** multiple clients concurrently.
**
** The game is called 'Master-Mind'.
** When a server starts to run, a secret code which consists of 4 capital letters is created
** and a client should challenge to guess the correct answer with 10 times opportunities.
** 
** Attribution: 
**  The basic socket programming code in this script was refered from the script 
**  'server.c' in TCP1 folder on unimelb LMS for COMP30023.
**
**  Building threads for multiple clients
**  http://www.binarytides.com/server-client-example-c-sockets-linux/
**
**  How to get memory usage information
**	http://stackoverflow.com/questions/1558402/memory-usage-of-current-process-in-c
**
** 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "server.h"


/* global variables used by signal handler function */
int num_connection = 0; 		/* number of successful connections */
int num_failure_contn = 0; 		/* number of failure connections */
int num_guess_correct = 0; 		/* number of clients who guessed successfully */
int num_guess_incorrect = 0;	/* number of clients who failed to guess right */
int fd; 						/* file descriptor for log.txt */

/* mutex lock */
pthread_mutex_t lock;


/*
** The main function which conducts the general functionalities of the server
*/
int main(int argc, char **argv) {

	/* variables for sockets */
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;

	/* server's secret code */
	char secret_code[5];


	/* handle SIGINT(Ctrl+C) signal */
	if(signal(SIGINT, signal_handler) == SIG_ERR)
		printf("\ncan't catch SIGALARM\n");

	/* create a log text file */
  	if((fd=open("./log.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU 
  	  				| S_IRWXG | S_IRWXO)) < 0) {
    	perror("open() failure");
    	exit(2);
    } 

	/* check the arguments */
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	/* extract the arguments */
	portno = atoi(argv[1]);


	/* set the secret code if given as an argument */
	if (argc == 3) {
		strcpy(secret_code, argv[2]);
	} else {
		strcpy(secret_code, "VOID");
	}


	/* initialise the mutex lock */
	 if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed\n");
        exit(1);
    }	


	 /* Create TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	
	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for 
	 this machine */	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format


	/* Bind address to the socket */
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}
	

	/* Listen on socket - means we're ready to accept connections - 
	 incoming connection requests will be queued */
	listen(sockfd, 5);
	
	clilen = sizeof(cli_addr);


	/* Accept a connection - block until a connection is ready to
	 be accepted. Get back a new file descriptor to communicate on. */
    puts("Ready to accept a connection. Now Waiting......");

	while ( (newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *) &clilen)) ) {
		puts("New client accepted");
		num_connection++;

        /* make a connection with this client by building a thread */ 
        pthread_t client_thread;

        /* build a struct argument for this thread */
        Thread_struct temp = {secret_code, fd, serv_addr, cli_addr, newsockfd};

        /* create a thread */
        if( pthread_create( &client_thread , NULL,  serve_client, (void*) &temp) < 0) {
            perror("could not create thread");
            return 1;
        }
         
        puts("Thread created for new client");
	}

	/* if the connection fails */
	if (newsockfd < 0) {
		num_failure_contn++;
		perror("ERROR on accepting this client");
	}


	/* close descriptors */
	close(sockfd);
	pthread_exit(NULL);
	
	return 0; 

}



/*
** The major function run by each thread.
*/
void *serve_client(void *param){

	/* scrape information from param */
	pthread_mutex_lock(&lock);

	Thread_struct *inform = (Thread_struct*) param;
	char secret_code[5];
	strcpy(secret_code, inform->secret_code);
	int fd = inform->fd;
	struct sockaddr_in serv_addr = inform->serv_addr;
	struct sockaddr_in cli_addr = inform->cli_addr;
	int newsockfd = inform->newsockfd;

	pthread_mutex_unlock(&lock);

	/* buffer to use when reading and writing */
	char buffer[BUFFER_SIZE];
	int num_correct, num_correct_letters;
	int n, i;


	/* if the secret code was not given as an argument, create one for this client */
	if(strcmp(secret_code, "VOID") == 0){
		generate_secret_code(secret_code);

	}


	/* messages */
	char* greetings = 
	"Welcome to Master Mind.\nGuess the secret code. You have 10 chances.\nThe code consists of [A,B,C,D,E,F] with length of 4.";
	char* guessed_right = "You guessed right. Well done.";
	char* guessed_wrong = "Try again.";
	char* run_out = "You tried 10 times. You lost.";
	char* invalid = "This input is not valid.";


	/* record the ip address of this client */
	record_message(fd, cli_addr, newsockfd, "client connected", "");

    /* record the secret code */
    record_message(fd, serv_addr, -1, "server secret = ", secret_code);


	/* greetings */
	bzero(buffer, BUFFER_SIZE);
	n = write(newsockfd, greetings, strlen(greetings));
	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
	


	/* Read the client's guess and check if it matches the secret code.
		They can try up to 10 times */
	for (i = 0 ; i < CHANCES; i++){

		/* read the guess */
		bzero(buffer, BUFFER_SIZE);
		n = read(newsockfd, buffer, BUFFER_SIZE-1);
		if (n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}

		/* record this guess in log.txt */
		record_message(fd, cli_addr, newsockfd, "client's guess = ", buffer);

		
		/* check if they match */
		if (strncmp(secret_code, buffer, strlen(secret_code)) == 0) {
			/* record this hint */
			record_message(fd, serv_addr, -1, "server's hint = [4:0]", "");

			/* increase the number of clients who guessed successfully */
			pthread_mutex_lock(&lock);
			num_guess_correct++;
			pthread_mutex_unlock(&lock);

			/* record this success */
			record_message(fd, cli_addr, newsockfd, "SUCCESS game over", "");

			/* send SUCCESS message to client */
			n = write(newsockfd, guessed_right, strlen(guessed_right));
			if (n < 0) {
				perror("ERROR writing to socket");
				exit(1);
			}

			break;

		} else {

			/* check if this was the last chance */		
			if (i == CHANCES -1){

				/* check invalidity of the input */
				if(!check_code(buffer)){
					/* record this invalidity */
					record_message(fd, serv_addr, -1, "server's reply = Invalid input", "");
				} else {
					/* get num_correct and num_correct_letters */
					generate_feedback(&num_correct, &num_correct_letters, secret_code, buffer);

					/* convert the two int values into string */
					char hint[6];
					sprintf(hint, "[%d:%d]", num_correct, num_correct_letters);

					/* record this hint */
					record_message(fd, serv_addr, -1, "server's hint = ", hint);
				}

				/* increase the number of clients who failed to guess right */
				pthread_mutex_lock(&lock);
				num_guess_incorrect++;
				pthread_mutex_unlock(&lock);

				/* record this failure */
				record_message(fd, cli_addr, newsockfd, "FAILURE game over", "");

				/* build a message to send to client */
				char client_lost[256];
				strcpy(client_lost, run_out);
				strcat(client_lost, " The secret code is ");
				strcat(client_lost, secret_code);
				strcat(client_lost, ".");

				/* send the msg to the client */
				n = write(newsockfd, client_lost, strlen(client_lost));
				if (n < 0) {
					perror("ERROR writing to socket");
					exit(1);
				}

			} else {
				/* check invalidity of the input */
				if(!check_code(buffer)){
					n = write(newsockfd, invalid, strlen(invalid));
					if (n < 0) {
						perror("ERROR writing to socket");
						exit(1);
					}

					/* record this invalidity */
					record_message(fd, serv_addr, -1, "server's reply = Invalid input", "");
					continue;

				}

				/* get num_correct and num_correct_letters */
				generate_feedback(&num_correct, &num_correct_letters, secret_code, buffer);

				/* convert the two int values into string */
				char hint[6];
				sprintf(hint, "[%d:%d]", num_correct, num_correct_letters);

				/* record this hint */
				record_message(fd, serv_addr, -1, "server's hint = ", hint);

				/* combine them into one string */
				char feedback[BUFFER_SIZE];
				strcpy(feedback, guessed_wrong);
				strcat(feedback, " Here is the feedback for you : ");
				strcat(feedback, hint);

				/* send a feedback to client */
				n = write(newsockfd, feedback, strlen(feedback));
				if (n < 0) {
					perror("ERROR writing to socket");
					exit(1);
				}
			}
				
		}

	}

	/* close this connection */
	close(newsockfd);

	return NULL;
}


/* handle SIGINT signal */
void signal_handler(int signal){
	
	char buffer[BUFFER_SIZE];
	char* line = "\n---------------------------------------\n";
	char* perform = "Performance Record\n";
	char* resource = "Resource Usage\n";

	/* record performance in log.txt */
	write(fd, line, strlen(line));
	write(fd, perform, strlen(perform));

	sprintf(buffer, "The number of successful connections: %d\n", num_connection);
	write(fd, buffer, strlen(buffer));

	sprintf(buffer, "The number of failed connections: %d\n", num_failure_contn);
	write(fd, buffer, strlen(buffer));

	sprintf(buffer, "The number of clients who guessed successfully: %d\n", num_guess_correct);
	write(fd, buffer, strlen(buffer));

	sprintf(buffer, "The number of clients who failed to guess : %d\n", num_guess_incorrect);
	write(fd, buffer, strlen(buffer));

	write(fd, line, strlen(line));


	/* record resource usage */
	write(fd, resource, strlen(resource));

	struct rusage usage;

    /*  get the resource usage */
    getrusage(RUSAGE_SELF, &usage);

    /* record the time spent both in usder mode and in kernel mode */
    sprintf(buffer, "The total amount of time spent executing in user mode is %lld milliseconds.\n",
    	(long long int)(int64_t)usage.ru_utime.tv_usec / 1000);
    write(fd, buffer, strlen(buffer));

    sprintf(buffer, "The total amount of time spent executing in kernel mode is %lld milliseconds.\n",
    	(long long int)(int64_t)usage.ru_stime.tv_usec / 1000);
    write(fd, buffer, strlen(buffer));


    /* record memory usage */
    FILE * stat_fp;
    long long vm_size;
    long long vm_rss;
    int page_size = getpagesize();

    /* open 'stamtm' file to scrape memory usage information */
   	stat_fp = fopen ("/proc/self/statm", "r");
  	if(stat_fp < 0){
    	perror("ERROR in opening statm file");
    	exit(1);
  	}

  	/* get memory usage info */
	fscanf(stat_fp, "%lld %lld", &vm_size, &vm_rss);

	/* close this file stream */
  	fclose(stat_fp);


  	/* record in log.txt */
  	sprintf(buffer, "The total virtual memory size is %lld kB.\n", vm_size*page_size/1024);
  	write(fd, buffer, strlen(buffer));

  	sprintf(buffer, "The total resident set size is %lld kB.\n\n", vm_rss*page_size/1024);
  	write(fd, buffer, strlen(buffer));


	/* close descriptor */
	close(fd);

	/* destroy the mutex */
	pthread_mutex_destroy(&lock);

    /* shut down the server */
	printf("\nServer is shutting down. Bye\n");
	exit(1);


}



/*
** generate a proper feedback according to client's guess
*/
void generate_feedback(int* num_correct, int* num_correct_letters, char* secret_code, char* code){

	/* flag array which indicates how many the 'char' is in the secret code. 
	the first flag refers to 'A' and the last flag refers to 'F' */
	int is_in_sec_code[6] = {0, 0, 0, 0, 0, 0};
	int i;

	*num_correct = 0;
	*num_correct_letters = 0;


	/* complete is_in_sec_code array */
	for(i = 0; i < strlen(secret_code); i++){
		is_in_sec_code[secret_code[i]-65]++;
	}

	/* check the correct colours in correct position */
	for(i = 0; i < strlen(secret_code); i++){
		if(code[i] == secret_code[i]){
			*num_correct = *num_correct + 1;
			is_in_sec_code[secret_code[i]-65]--;
		}

	}

	/* check the colours in wrong position */
	for(i = 0; i < strlen(secret_code); i++){
		if(code[i] != secret_code[i] && is_in_sec_code[code[i]-65] > 0){
			*num_correct_letters = *num_correct_letters + 1;
			is_in_sec_code[code[i]-65]--;
		}		
	}

}


/*
** record the given message with its arguments 
*/
void record_message(int fd, struct sockaddr_in addr, int id, char* msg, char* arg){

	record_time(fd);

	/* lock to protect the integrity of the log file */
	pthread_mutex_lock(&lock);
	
	char buffer_log[BUFFER_SIZE];
	char* ip = inet_ntoa(addr.sin_addr);
	int rc;

	/* build a meesage */
	if(id < 0){
		sprintf(buffer_log, " (%s) %s%s\n", ip, msg, arg);
	} else {
		if(strcmp(msg, "client's guess = ") == 0)
			sprintf(buffer_log, " (%s)(%d) %s%s", ip, id, msg, arg);
		else
			sprintf(buffer_log, " (%s)(%d) %s%s\n", ip, id, msg, arg);
	}
		
	/* record this message in log.txt */
    rc = write(fd, buffer_log, strlen(buffer_log));
    assert(rc == (strlen(buffer_log)));

    pthread_mutex_unlock(&lock);


}


/*
** generate a random secret code which consists of 4 capital letters
** {A, B, C, D, E, F}
*/
void generate_secret_code(char* secret_code){
	int i;
	for(i = 0; i < strlen(secret_code) ; i++)
		secret_code[i] = rand() % 6 + 65;

}


/*
** check if the given code is appropriate (if they consist of A,B,C,D,E or F)
** if appropriate, return 1 otherwise 0
*/
int check_code(char* code){
	int i;
	for (i = 0; i < 4; i++) {
		if (code[i] < 65 || code[i] > 70)
			return 0;
	}
	return 1;
}


/* 
** wrtie the current time into the given fd(file descriptor)
*/
void record_time(int fd){

	/* lock to protect the integrity of the log file */
    pthread_mutex_lock(&lock);

	int n;
	/* variables for getting the current time */
	time_t t = time(NULL);;
    struct tm *tm = localtime(&t);
    
    n = write(fd, asctime(tm), strlen(asctime(tm))-1);
    if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
    
    pthread_mutex_unlock(&lock);


}





