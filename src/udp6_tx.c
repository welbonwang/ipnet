#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MAXBUF 1024

int main(int argc, char **argv)
{
	int sockfd;
	socklen_t len;
	struct sockaddr_in6 addr;

	unsigned int myport, lisnum;
	char buf[MAXBUF + 1];

	if (argv[1])
		myport = atoi(argv[1]);
	else
		myport = 7838;

	sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket");
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin6_family = PF_INET6;
	addr.sin6_port = htons(myport);
	if (argv[2])
		inet_pton(AF_INET6, argv[2], &addr.sin6_addr);
	else
		addr.sin6_addr = in6addr_any;

	bzero(buf, MAXBUF + 1);
	strcpy(buf, "Hello UDP based on IPv6\n");
	len = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&addr, sizeof(addr));
	if (len > 0)
		printf("Sent: %s", buf);

	close(sockfd);
	return 0;
}
