#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>
#include <winsock2.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>

/**
 * hInstance: HInstance is a handle needed by window creation, menus, and a whole host of 
 * other functions to identify the correct program and instance when passing
 * commands or requesting data.
 * 
 * hPrev: used in 16bit windows, now 0
 * lpCmdLine: contains command line arguments
 * nCmdShow: flag for main application window state (minimized, maximized or normal)
***/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    // hide the window
    HWND stealth;
    AllocConsole();
    stealth = FindWindow("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);
}