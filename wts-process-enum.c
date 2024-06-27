#include <windows.h>
#include <wtsapi32.h>
#include <stdio.h>

int main(int argc, char **argv) {
    CHAR *hostname = argv[1];
    CHAR *domain = argv[2];
    CHAR *username = argv[3];
    CHAR *password = argv[4];
    HANDLE hServer = WTSOpenServerA(hostname);
    WTS_PROCESS_INFOA *wpi = NULL;
    DWORD dwCount = 0;
    BOOL bResult = FALSE;

    HANDLE hToken = NULL;
    bResult = LogonUserA(username, domain, password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_DEFAULT, &hToken);
    if(!bResult) {
        printf("LogonUserA failed %ld\n", GetLastError());
        ExitProcess(0);
    }

    bResult = FALSE;
    bResult = ImpersonateLoggedOnUser(hToken);
    if(!bResult) {
        printf("ImpersonateLoggedOnUser failed %ld\n", GetLastError());
        ExitProcess(0);
    }

    printf("hServer HANDLE is 0x%p\n", hServer);

    WTSEnumerateProcessesA(hServer, 0, 1, &wpi, &dwCount);

    printf("We got %d processes.\n", dwCount);

    DWORD i = 0;
    for(i; i < dwCount; i++) {
        printf("%d %s\n", wpi[i].ProcessId, wpi[i].pProcessName);
    }

    WTSCloseServer(hServer);

    return 0;
}
