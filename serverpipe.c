#include <Windows.h>
#include <stdio.h>

#define MAX_SIZE 1024

int main() {
    CHAR buffer[MAX_SIZE];
    DWORD dwRead = 0;
    HANDLE hPipe = CreateNamedPipe("\\\\.\\pipe\\ringzer0", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, PIPE_UNLIMITED_INSTANCES, MAX_SIZE, 0, 10000, NULL);
    printf("hPipe 0x%p\n", hPipe);

    ConnectNamedPipe(hPipe, NULL);

    ReadFile(hPipe, buffer, MAX_SIZE, &dwRead, NULL);

    printf("We got %d bytes\n", dwRead);
    printf("Received: %s\n", buffer);

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    return 0;
}
