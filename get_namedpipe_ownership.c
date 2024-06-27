#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv) {
    LONG PID = 0;
    CHAR *pipename = argv[1];
    HANDLE hPipe = CreateFile(pipename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    printf("HANDLE to %s is 0x%p\n", pipename, hPipe);
    GetNamedPipeServerProcessId(hPipe, &PID);
    printf("NamedPipe is owned by PID %d\n", PID);
    return 0;
}
