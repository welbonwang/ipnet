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
#include <netdb.h>

#define MAXBUF 1024

/** L2TPv3 control message header structure
 *
 * This structure is defined for little endian system.
 * len, conn_id, ns and nr are all network order bytes.
 *
 * @len: length of L2TPv3 control message including this header and AVPs
 * @conn_id: control connection id
 * @ns: send number
 * @nr: receiver number
 */
typedef struct __l2tpv3_ctrl_msg_hdr {
	uint8_t rsv3:3;
	uint8_t s:1;
	uint8_t rsv2:2;
	uint8_t l:1;
	uint8_t t:1;
	uint8_t ver:4;
	uint8_t rsv4:4;
	uint16_t len;
	uint32_t conn_id;
	uint32_t ns, nr;
	char avps[];
} __attribute__((packed)) l2tpv3_cmsg_hdr_t;

int main(int argc, char **argv)
{
	int sockfd, ecode;
	int idx;
	ssize_t len;
	struct sockaddr_l2tpip6 addr;
        struct addrinfo *ai_got, ai_hint;
	l2tpv3_cmsg_hdr_t hdr = {
		.t = 1,
		.l = 1,
		.s = 1,
		.ver = 3
	};

	unsigned int conn_id;
	char buf[MAXBUF + 1];

	if (argv[1])
		conn_id = atoi(argv[1]);
	else
		conn_id = 7838;

	sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_L2TP);
	if (sockfd == -1) {
		perror("socket");
		exit(1);
	}

        bzero(&ai_hint, sizeof(ai_hint));
        ai_hint.ai_family = AF_INET6;
        ai_hint.ai_socktype = SOCK_DGRAM;

	bzero(&addr, sizeof(addr));
	addr.l2tp_family = PF_INET6;
	addr.l2tp_conn_id = conn_id;
	if (argv[2]) {
		struct sockaddr_in6 *sa_got;
                ecode = getaddrinfo(argv[2], NULL, &ai_hint, &ai_got);
		if (ecode) {
			perror("getaddrinfo");
			exit(1);
		}
		sa_got = (struct sockaddr_in6 *)ai_got->ai_addr;
		addr.l2tp_addr = sa_got->sin6_addr;
		addr.l2tp_scope_id = sa_got->sin6_scope_id;
		freeaddrinfo(ai_got);
		ai_got = NULL;
	} else
		addr.l2tp_addr = in6addr_any;

	bzero(buf, MAXBUF + 1);
	char AVPs[] = "Hello UDP based on IPv6\n";
	hdr.ns = htonl(0);
	hdr.nr = htonl(0);
	hdr.len = htons(sizeof(hdr) + strlen(AVPs));
	idx = 0;
	memcpy(&buf[idx], &hdr, sizeof(hdr));
	idx = sizeof(hdr);
	memcpy(&buf[idx], AVPs, strlen(AVPs));
	idx += strlen(AVPs);
	len = sendto(sockfd, buf, idx, 0, (struct sockaddr *)&addr, sizeof(addr));
	if (len > 0)
		printf("Sent %ld bytes: %s", len, AVPs);

	close(sockfd);
	return 0;
}
