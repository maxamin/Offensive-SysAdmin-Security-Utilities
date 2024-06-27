#include <windows.h>
#include <stdio.h>

VOID CreateSlot(CHAR *slot, HANDLE *hSlot) {
    *hSlot = CreateMailslot(slot, 0, MAILSLOT_WAIT_FOREVER, NULL);
}

VOID ReadFromSlot(HANDLE hSlot) {
    DWORD lpNextSize = 0;
    DWORD lpMessageCount = 0;

    BOOL bSuccess = GetMailslotInfo(hSlot, NULL, &lpNextSize, &lpMessageCount, NULL);

    if(!bSuccess) {
        printf("GetMailslotInfo call failed %d\n", GetLastError());
    }

    if(lpMessageCount == MAILSLOT_NO_MESSAGE) {
        printf("we don't have message\n");
    }
    
    printf("We got %d message\n", lpMessageCount);

    /*
    do {}
    call GetMailslotInfo
    } while(lpMessageCount != 0)
    */

    while(lpMessageCount != 0) {
        DWORD dwRead = 0;
        CHAR *message = (CHAR*)GlobalAlloc(GPTR, lpNextSize + 1);
        printf("Allocation %d bytes\n", lpNextSize);
        ReadFile(hSlot, message, lpNextSize, &dwRead, NULL);
        printf("message is %s\n", message);
        GlobalFree(message);
        bSuccess = GetMailslotInfo(hSlot, NULL, &lpNextSize, &lpMessageCount, NULL);
        printf("We got %d message\n", lpMessageCount);
    }
}

int main(int argc, char **argv) {
    CHAR slot[] = "\\\\.\\mailslot\\hacker";
    HANDLE hSlot = NULL;
    CreateSlot(slot, &hSlot);
    printf("HANDLE is %p\n", hSlot);
    HANDLE hMail = CreateFile(slot, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwWritten = 0;
    printf("HANDLE is %p\n", hMail);

    // execute something evil and get the output back the WriteFile
    // HUGE Warning mailslot size is extremely limited
    // 1-2:test
    // 2-2:moretest
    WriteFile(hMail, argv[1], strlen(argv[1]), &dwWritten, NULL);

    ReadFromSlot(hSlot);
    CloseHandle(hMail);
    CloseHandle(hSlot);

    return 0;
}
