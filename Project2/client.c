/* Student Name : Tessa(Hyeri) Song
** Login id : songt
** Fri 27 May 2016
**
** This script is for implementation of 'client' which communicates with
** a simple game server
**
** The game is called 'Master-Mind'.
** When a server starts to run, a secret code which consists of 4 capital letters is created
** and a client should challenge to guess the correct answer with 10 times opportunities.
** 
** Attribution: 
**  The basic socket programming code in this script was refered from the script 
**  'client.c' in TCP1 folder on unimelb LMS for COMP30023.
** 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "client.h"


/*
** The main function which conducts the general functionalities of a client
*/
int main(int argc, char**argv)
{
	/* variables for sockets */
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	/* buffer to use when reading and writing */
	char buffer[BUFFER_SIZE];

	/* messages */
	char* guessed_right = "You guessed right. Well done.";
	char* run_out = "You tried 10 times. You lost.";

	/* check the arguments */
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	/* extract the port number */
	portno = atoi(argv[2]);
	
	/* Translate host name into peer's IP address ;
	 * This is name translation service by the operating system 
	 */
	server = gethostbyname(argv[1]);
	
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	

	/* Building data structures for socket */
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);

	serv_addr.sin_port = htons(portno);


	/* Create TCP socket -- active open 
	* Preliminary steps: Setup: creation of active open socket
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(0);
	}
	
	if (connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(0);
	}


	/* accept greetings */
	bzero(buffer, BUFFER_SIZE);
	n = read(sockfd, buffer, BUFFER_SIZE-1);
	if (n < 0){
			perror("ERROR reading from socket");
			exit(0);
	}

	printf("%s\n", buffer);



	/* Start guessing the secret code. Up to 10 times
	*/
	int i = 10;
	while (TRUE) {
		printf("%d chance(s) left. Please guess the secret code: ", i);
		i--;

		/* send the guess to game server */
		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE-1, stdin);

		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) {
			perror("ERROR writing to socket");
			exit(0);
		}
	

		/* receive the reply from the server */
		bzero(buffer, BUFFER_SIZE);
		n = read(sockfd, buffer, BUFFER_SIZE-1);
		if (n < 0){
			perror("ERROR reading from socket");
			exit(0);
		}

		printf("%s\n",buffer);


		/* if the guess is correct */
		if(strncmp(guessed_right, buffer, strlen(guessed_right)) == 0)
			break;

		/* if the chances run out */
		if(strncmp(run_out, buffer, strlen(run_out)) == 0)
			break;
		
	}

	return 0;
}





