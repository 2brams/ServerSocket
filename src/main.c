#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "actionneur.h"
#include "capteur.h"

int main(int argc, char **argv) {

	struct addrinfo hints, *server;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE || SOCK_NONBLOCK;
	getaddrinfo(NULL, "8888", &hints, &server);

	int sockfd = socket(server->ai_family, server->ai_socktype,
			server->ai_protocol);

	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	} else
		printf("Socket successfully created..\n");

	if ((bind(sockfd, server->ai_addr, server->ai_addrlen)) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	} else
		printf("Socket successfully binded..\n");

	if ((listen(sockfd, 10)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	} else
		printf("Server listening..\n");

	struct sockaddr_storage client_addr;
	socklen_t addr_size = sizeof client_addr;
	char buffer[200];
	char data[500];

	if (wiringPiSetup() == -1)
		return 1;
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	digitalWrite(0, 0);
	digitalWrite(1, 0);
	for (;;) {

		int client_fd = accept(sockfd, (struct sockaddr*) &client_addr,
				&addr_size);

		if (client_fd < 0) {
			printf("server acccept failed...\n");
			exit(0);
		} else {
			printf("server acccept the client...\n");
			digitalWrite(0, 1);

			int n = read(client_fd, buffer, 200);
			printf("%s", buffer);

			if (strncmp("on", buffer, 2) == 0) {
				snprintf(data, sizeof data, "%d", blink());
			} else if (strncmp("off", buffer, 3) == 0) {
				snprintf(data, sizeof data, "%d", offBlink());
			} else if (strncmp("cpt", buffer, 3) == 0) {

				const char *datas = bme280();
				snprintf(data, sizeof data, "%s", datas);
				free(datas);

			} else if (strncmp("tmp", buffer, 3) == 0) {
				snprintf(data, sizeof data, "%6.3f", cpuTemp());
			} else if (strncmp("frq", buffer, 3) == 0) {
				snprintf(data, sizeof data, "%6.3f", cpuFreq());
			} else {
				snprintf(data, sizeof data, "%s\n %s\n", "OK:", buffer);
			}
			printf("\n");
			n = write(client_fd, data, strlen(data));
			close(client_fd);
			digitalWrite(0, 0);
		}

	}
	digitalWrite(1, 0);
	return (EXIT_SUCCESS);
}
