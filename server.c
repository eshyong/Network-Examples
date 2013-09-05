#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define BACKLOG 10
#define MESSAGE_SIZE 256
#define PORT 80
// 127.0.0.1
#define LOCAL_HOST 2130706433 

void reap(void) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
	// socket variables
	int ssock, csock;
	socklen_t sock_addrlen;
	struct sockaddr_in sa;

	// used when forking
	int pid;
	int status;

	// message and its size
	int bytes;
	char msg[MESSAGE_SIZE];

	// reap all zombie processes
	signal(SIGCHLD, (void *)reap);

	// create a socket
	if ((ssock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

	// set protocol, port, and address
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(LOCAL_HOST);

	// bind socket to address
	if (bind(ssock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		return 2;
	}

	printf("waiting for connections...\n");

	// set socket to listen
	listen(ssock, BACKLOG);

	while (1) {
		// address length, passed by reference
		sock_addrlen = sizeof(sa);

		// wait for clients to connect
		if ((csock = accept(ssock, (struct sockaddr *)&sa, &sock_addrlen)) < 0) {
			perror("server: accept");
			return 3;
		}

		printf("connection accepted from %s\n", inet_ntoa(sa.sin_addr));

		// fork a child
		switch (pid = fork()) {
			// fork error
			case -1:
				perror("fork");
				return 4;
			// child process
			case 0: {
				// child doesn't need server socket
				close(ssock);

				// recv return value
				printf("waiting for message...\n");

				// message received with error handling
				while (1) {
					// clear msg array
					memset(msg, 0, sizeof(msg) / sizeof(char));

					// recv call
					bytes = recv(csock, msg, MESSAGE_SIZE, 0);

					// print received messages
					if (strcmp(msg, "") > 0) {
						printf("received message: %s\nlength: %d\n", msg, bytes);
					}

					// exit if client disconnects
					if (strcmp(msg, "exit") == 0) {
						printf("client exiting...\n");
						exit(0);
					}

					// print error, if any
					if (bytes < 0) {
						perror("recv");
						continue;
					}
				}
				break;
			}
			// parent process
			default: {
				// wait for child process to end
				wait(&status);
				printf("exiting with status %d\n", status);

				// close sockets and exit
				close(ssock);
				close(csock);
				exit(status);
			}
		}

		break;
	}


	return 0;
}

