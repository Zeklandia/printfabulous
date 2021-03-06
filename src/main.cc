#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fstream>

#define PORT 9100

using namespace std;

int main (int argc, char *argv[]) {
	int sockfd, len, bytes_sent;
	struct hostent *host;
	struct sockaddr_in target_addr;
	char line[100];

	string success ("\e[1;30m[\e[0;32mDone\e[1;30m]\e[0m");
	string fail ("\e[1;30m[\e[0;31mFail\e[1;30m]\e[0m");
	
	if (argc != 4) {
	printf("HP Printer Message Editor\n"																);
	printf("%s [Printer IP Address] \"String\" [rdy (ready message) or err (error message) or op (operator message))]\n", argv[0] );
	printf("Message can be up to 16 characters long (32 on 2 line displays)\n"							);
	exit(1);
	}

	if ((host = gethostbyname(argv[1])) == NULL) {
		cout << "Hostname                                                                  " << fail << endl;
		exit(1);
	}

	printf ("HP Printer Message Editor\n");
	printf ("Hostname:\t%s\n", argv[1]);
	printf ("Message Type:\t%s\n", argv[3]);
	printf ("Message:\t%s\n", argv[2]);

	target_addr.sin_family = host -> h_addrtype;
	target_addr.sin_port = htons(PORT);

	bcopy(host -> h_addr, (char *) &target_addr.sin_addr, host -> h_length);
	bzero(&(target_addr.sin_zero), 8);
	
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		cout << "Socket                                                                    " << fail << endl;
		exit(-1);
	}

	printf ("Connecting...\n");
	
	if (connect(sockfd, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1) {
		cout << "Connect                                                                   " << fail << endl;
		exit(-1);
	}

	char rdy[] = "rdy";
	char err[] = "err";
	char op[] = "op";
	
	if (strcmp(argv[3], rdy) == 0) {
		strcat(line, "\e%-12345X@PJL JOB\n@PJL RDYMSG DISPLAY=\"");
	}
	else if (strcmp(argv[3], err) == 0) {
		strcat(line, "\e%-12345X@PJL JOB\n@PJL ERRMSG DISPLAY=\"");
	}
	else if (strcmp(argv[3], op) == 0) {
		strcat(line, "\e%-12345X@PJL JOB\n@PJL OPMSG DISPLAY=\"");
	}
	
	strcat(line, argv[2]);
	strcat(line, "\"\n@PJL EOJ\n\e%-12345X\n");
	bytes_sent = send(sockfd, line, strlen(line), 0);
	printf ("Packet:\t%s\n", line);
	printf("Sent %d bytes\n", bytes_sent);
	close(sockfd);
}