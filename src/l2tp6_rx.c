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
#include <linux/l2tp.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
	int sockfd;
	int ecode, pkt_cnt = 0;
	ssize_t len;
	struct sockaddr_l2tpip6 addr;

	uint32_t conn_id;
	char buf[MAXBUF + 1];
	char outstr[MAXBUF + 1];

	if (argv[1])
		conn_id = atoi(argv[1]);
	else
		conn_id = 7838;

	sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_L2TP);
	if (sockfd == -1) {
		perror("fail to create socket");
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.l2tp_family = PF_INET6;
	addr.l2tp_conn_id = conn_id;
	if (argv[2])
		inet_pton(AF_INET6, argv[2], &addr.l2tp_addr);
	else
		addr.l2tp_addr = in6addr_any;

	ecode = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if (ecode < 0) {
		perror("bind fail");
		exit(1);
	}

	while (1) {
		len = recv(sockfd, buf, sizeof(buf), 0);
		if (len > 0) {
			pkt_cnt++;
			fprintf(stdout, "%03d, %ld-byte: ", pkt_cnt, len);
			fwrite(buf, sizeof(char), len, stdout);
			fflush(stdout);
		} else
			printf("receive unexpected data\n");
	}

	close(sockfd);

	return 0;
}
