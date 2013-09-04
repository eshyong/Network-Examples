#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
	struct sockaddr_in sa;
	char ip[INET_ADDRSTRLEN];
	sa.sin_addr.s_addr = htonl(2130706433);
	inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

	printf("%s\n", ip);

	return 0;
}