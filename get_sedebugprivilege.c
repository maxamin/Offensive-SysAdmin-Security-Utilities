#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv) {
    // GetCurrentProcess() return handle on your own process
    DWORD PID = atoi(argv[1]);
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;

    printf("Remote process HANDLE %p\n", hProc);

    OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    printf("Remote process token HANDLE %p\n", hToken);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    LookupPrivilegeValueA(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);

    AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

    if(GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        printf("Granting SeDebugPrivilege failed\n");
    } else {
        printf("You know have the SeDebugPrivilege\n");
    }

    CloseHandle(hToken);
    CloseHandle(hProc);

    return 0;
}
