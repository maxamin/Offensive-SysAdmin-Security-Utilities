#include <Windows.h>
#include <stdio.h>

#define BUFFER_SIZE 255

VOID GetWindowClass(HWND hw) {
    CHAR data[BUFFER_SIZE + 1];
    GetClassName(hw, data, BUFFER_SIZE);
    printf("HWND %p ClassName is %s\n", hw, data);

    memset(data, 0x00, BUFFER_SIZE + 1);
    GetWindowText(hw, data, BUFFER_SIZE);
    printf("HWND %p WindowText is %s\n", hw, data);
}

int main(int argc, char **argv) {
    DWORD PID = atoi(argv[1]);
    HWND hw = NULL;
    DWORD dwWindowPID = 0;
    do
    {
        hw = FindWindowEx(NULL, hw, NULL, NULL);
        GetWindowThreadProcessId(hw, &dwWindowPID);
        if(dwWindowPID == PID) {
            printf("HWND %p belong to PID %d\n", hw, PID);
            GetWindowClass(hw);
        }
    }
    while (hw != NULL);

    return 0;
}

