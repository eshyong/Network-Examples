#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 80
// 127.0.0.1
#define LOCAL_HOST 2130706433

int main() {
	int sock;
	int bytes;
	struct sockaddr_in sa;
	char buffer[BUFFER_SIZE + 1];

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(LOCAL_HOST);
	if (connect(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("connect");
		close(sock);
		return 2;
	}

	while ((bytes = read(sock, buffer, BUFFER_SIZE)) > 0) {
		write(1, buffer, bytes);
	}

	close(sock);

	return 0;
}