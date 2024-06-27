#include <windows.h>
#include <stdio.h>
#include <winnt.h>

int main(int argc, char **argv) {
    HANDLE hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD dwSize = GetFileSize(hFile, NULL);

    CHAR *buffer = GlobalAlloc(GPTR, dwSize);
    DWORD dwOld = 0;
    DWORD dwRead = 0;
    VirtualProtect(buffer, dwSize, PAGE_EXECUTE_READWRITE, &dwOld);
    ReadFile(hFile, buffer, dwSize, &dwRead, NULL);

    void (*function)();
    function = (void (*)())buffer;
    function();

    return 0;
}
