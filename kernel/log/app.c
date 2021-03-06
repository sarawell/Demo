#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/netlink.h>
#define MAX_LEN 1024
#define NETLINK_ZSLOG 30
void main() {
	int sockfd = 0, len = 0;
	struct sockaddr_nl sn;
	struct sockaddr server;
	char buff[MAX_LEN];
	sockfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ZSLOG);
	if(sockfd < 0) {
		printf("create socket FAILED, error:%s\n", strerror(errno));
		exit(-1);
	}
	sn.nl_family = AF_NETLINK;
	sn.nl_pad = 0;

	if(bind(sockfd, (struct sockaddr*)&sn, sizeof(struct sockaddr)) < 0) {
		printf("create socket FAILED, error:%s\n", strerror(errno));
		exit(-1);
	}
	while(1) {
		recvfrom(sockfd, buff, MAX_LEN, 0, &server, &len);
		printf("%s\n", buff);
	}
}
