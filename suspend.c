/*
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if(Thread32First(hSnap, &te)) {
        do {
            if(te.th32OwnerProcessID == PID) {
                snprintf(log, 255, "FOUND Thread %d for process with PID %d\n", te.th32ThreadID, te.th32OwnerProcessID);
                Log(&log);
                HANDLE hThread = NULL;
                hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if(hThread == NULL) {
                    snprintf(log, 255, "Failed to OpenThread. Error %d\n", GetLastError());
                    Log(&log);
                } else {
                    DWORD dwCount = SuspendThread(hThread);
                    snprintf(log, 255, "Suspend state %d for thread ID %d\n", dwCount, te.th32OwnerProcessID);
                    Log(&log);
                }
            }
        } while(Thread32Next(hSnap, &te));
    }
*/

#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv) {
	HANDLE hProc = OpenProcess(PROCESS_SUSPEND_RESUME, TRUE, atoi(argv[1]));
	printf("HANDLE VALUE 0x%p\n", hProc);
	
	FARPROC NtSuspendProcess = GetProcAddress(LoadLibrary("ntdll.dll"), "NtSuspendProcess");

	NtSuspendProcess(hProc);

	return 0;
}
