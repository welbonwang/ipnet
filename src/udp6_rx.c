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
	int ecode, msg_cnt = 0;
	ssize_t len;
	struct sockaddr_in6 addr;

	unsigned int myport;
	char buf[MAXBUF + 1];
	char outstr[MAXBUF + 1];

	if (argv[1])
		myport = atoi(argv[1]);
	else
		myport = 7838;

	sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("fail to create socket");
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin6_family = PF_INET6;
	addr.sin6_port = htons(myport);
	if (argv[2])
		inet_pton(AF_INET6, argv[2], &addr.sin6_addr);
	else
		addr.sin6_addr = in6addr_any;

	ecode = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if (ecode < 0) {
		perror("bind fail");
		exit(1);
	}

	while (1) {
		len = recv(sockfd, buf, sizeof(buf), 0);
		if (len > 0) {
			msg_cnt++;
			fprintf(stdout, "%03d: ", msg_cnt);
			fwrite(buf, sizeof(char), len, stdout);
			//snprintf(outstr, 20, "%s\n", buf);
			//fprintf("%s\n", outstr);
			fflush(stdout);
		} else
			printf("receive unexpected data\n");
	}

	close(sockfd);

	return 0;
}
