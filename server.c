#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define bzeroWin(p, size) (void) memset((p), 0, (size))
#define CONN_AMNT 5
#define TRUE 1

int main() {
    int sock, clientSocket;
	char buffer[1024];
	char response[18384];
	struct sockaddr_in serverAddress, clientAddress;
	int i = 0, optVal = 1;
    char *servIP;
    unsigned short servPort;
	socklen_t clientLength;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
		printf("\nError: Setting TCP Socket Options ;(\n");
		return 1;
	}

    servIP = "192.168.43.31";
    servPort = 50005;
    serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(servIP);
	serverAddress.sin_port = htons(servPort);

    bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	listen(sock, CONN_AMNT);
    clientLength = sizeof(clientAddress);
	clientSocket = accept(sock, (struct sockaddr *) &clientAddress, &clientLength);

    while (TRUE) {
        bzero(&buffer, sizeof(buffer));
		bzero(&response, sizeof(response));
        // adaption for windows mchine
        // bzeroWin(&buffer, sizeof(buffer));
		// bzeroWin(&response, sizeof(response));
		printf("[%s]-[cmd]~$: ", inet_ntoa(clientAddress.sin_addr));
		fgets(buffer, sizeof(buffer), stdin);
        strtok(buffer, "\n");
		write(clientSocket, buffer, sizeof(buffer));

        if (strncmp("q", buffer, 1) == 0) {
			break;
            // WSACleanup();
		} else {
			recv(clientSocket, response, sizeof(response), MSG_WAITALL);
			printf("%s", response);
		}
    }
}