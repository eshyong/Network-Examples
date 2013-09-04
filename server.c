#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 10
#define PORT 80
// 127.0.0.1
#define LOCAL_HOST 2130706433 

int main() {
	int ssock, csock;
	socklen_t sock_addrlen;
	struct sockaddr_in sa;
	time_t t;
	struct tm *tm;
	FILE *client;

	if ((ssock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(LOCAL_HOST);

	if (bind(ssock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		return 2;
	}

	switch (fork()) {
		case -1:
			perror("fork");
			return 3;
		case 0:
			break;
		default:
			close(ssock);
			return 0;
	}

	listen(ssock, BACKLOG);

	for (;;) {
		sock_addrlen = sizeof(sa);

		if ((csock = accept(ssock, (struct sockaddr *)&sa, &sock_addrlen)) < 0) {
			perror("ssock: accept");
			return 4;
		}

		if ((client = fdopen(csock, "w")) == NULL) {
			perror("fdopen");
			return 5;
		}

		if ((t = time(NULL)) < 0) {
			perror("time");
			return 6;
		}

		tm = gmtime(&t);
		fprintf(client, "%.4i-%.2i-%.2iT%.2i:%.2i:%.2iZ\n",
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec);

		fclose(client);
	}

	return 0;
}

