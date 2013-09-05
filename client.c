#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define PORT 8000
// 127.0.0.1
#define LOCAL_HOST 2130706433

int main() {
	// socket variables
	int sock;
	int bytes;
	struct sockaddr_in sa;

	// buffer to hold messages
	char buffer[BUFFER_SIZE];

	// create a socket
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

	// store family, port, and address
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(LOCAL_HOST);

	// connect socket to address
	if (connect(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("connect");
		close(sock);
		return 2;
	}

	printf("connected to 127.0.0.1\n");

	// loop to send messages in
	while (1) {
		// get user input
		fgets(buffer, BUFFER_SIZE, stdin);

		// remove newline character
		for (int i = 0; i < BUFFER_SIZE; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
				break;
			}
		}

		// print out error if any
		if ((bytes = send(sock, buffer, strlen(buffer), 0)) < 0) {
			perror("send");
		}

		// print length of message
		printf("bytes sent: %d\n", bytes);

		// exit message
		if (strcmp(buffer, "exit") == 0) {
			printf("exiting...\n");
			break;
		}
	}

	close(sock);

	return 0;
}