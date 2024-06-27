#include <Windows.h>
#include <stdio.h>

#define MAX_SIZE 1024

int main(int argc, char **argv) {
    CHAR *remotePipeName = (CHAR*)GlobalAlloc(GPTR, MAX_SIZE);
    DWORD dwWritten = 0;
    snprintf(remotePipeName, MAX_SIZE, "\\\\%s\\pipe\\%s", argv[1], argv[2]);
    printf("Connecting to %s\n", remotePipeName);

    HANDLE hPipe = CreateFile(remotePipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    printf("hPipe 0x%p\n", hPipe);

    WriteFile(hPipe, argv[3], strlen(argv[3]), &dwWritten, NULL);

    CloseHandle(hPipe);

    return 0;
}
