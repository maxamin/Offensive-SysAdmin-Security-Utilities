#define SECURITY_WIN32
#define __WIN32_WINNT 0x0A00

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <secext.h>
#include <intrin.h>
#include <sysinfoapi.h>

#define MAX_BUFFER_SIZE 512

// check for specific running process
BOOL CheckRunningApp(CHAR *app) {
    BOOL bResult = FALSE;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hSnap, &pe32)) {
        do {
            if(strcmp(pe32.szExeFile, app) == 0) {
                bResult = TRUE;
                break;
            }
        } while(Process32Next(hSnap, &pe32));
    }

    CloseHandle(hSnap);

    return bResult;
}

BOOL CheckDomain(CHAR *domain) {
    // NetGetJoinInformation which establish network connection

    // CHAR buffer[256];
    // GetEnvironmentVariable("USERDOMAIN", buffer, 256);
    BOOL bResult = FALSE;
    CHAR buffer[MAX_BUFFER_SIZE];
    DWORD dwSize = MAX_BUFFER_SIZE;
    GetUserNameExA(NameSamCompatible, buffer, &dwSize);
    CHAR *position = strstr(buffer, "\\");

    position[0] = 0x00;
    if(strcmp(domain, buffer) == 0) {
        bResult = TRUE;
    }

    return bResult;
}

BOOL CheckTime(DWORD milliseconds) {
    BOOL bResult = FALSE;
    DWORD dwStart = GetTickCount();
    SleepEx(milliseconds, FALSE);
    DWORD dwEnd = GetTickCount();
    if(dwEnd - dwStart >= milliseconds) {
        bResult = TRUE;
    }
    return bResult;
}

BOOL GetMemory(DWORD dwSize) {
    BOOL bResult = FALSE;
    DWORD64 dwTotalMem;
    FARPROC GetPhysicallyInstalledSystemMemory = GetProcAddress(LoadLibrary("kernel32.dll"), "GetPhysicallyInstalledSystemMemory");
    GetPhysicallyInstalledSystemMemory(&dwTotalMem);
    if(dwTotalMem / (1024*1024) >= dwSize) {
        bResult = TRUE;
    }
    return bResult;
}

BOOL GetDisk(DWORD dwSize) {
    BOOL bResult = FALSE;
    DWORD64 lpFreeBytesAvailableToCaller;
    DWORD64 lpTotalNumberOfBytes;
    DWORD64 lpTotalNumberOfFreeBytes;
    FARPROC GetDiskFreeSpaceExA = GetProcAddress(LoadLibrary("kernel32.dll"), "GetDiskFreeSpaceExA");
    GetDiskFreeSpaceExA("C:", &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

    DWORD dwDiskSize = (lpFreeBytesAvailableToCaller / (1024*1024)) + (lpTotalNumberOfBytes / (1024 * 1024));
    if(dwDiskSize >= dwSize) {
        bResult = TRUE;
    }
    return bResult;
}

BOOL CheckCPUID(DWORD contains) {
    // https://en.wikipedia.org/wiki/CPUID
    BOOL bResult = FALSE;
    int index[4];

    /*
    DWORD64 *index;
    index = GlobalAlloc(GPTR, sizeof(DWORD64) * 4);
    */
    int id = 0;
    __cpuid(index, id);
    if(index[1] == contains) {
        bResult = TRUE;
    }
    
    /*
    int level; 0;

    asm("cpuid\n\t"					
	    : "=a" (index[0]), "=b" (index[1]), "=c" (index[2]), "=d" (index[3])
        : "0" (level)
    );
    */

    return bResult;
}

int main() {

    if(!CheckRunningApp("notepad.exe")) {
        printf("Notepad.exe not running this system is hostile.\n");
    } else {
        // do something evil
    }

    if(!CheckDomain("RINGZER0")) {
        printf("Current user not part of the RINGZER0 domain.\n");
    } else {
        // do something evil
    }

    if(!CheckTime(10)) {
        printf("Time is moving hella fast.\n");
    }

    if(!GetMemory(128)) {
        printf("Computer have less than 128 GB you poor fella.\n");
    }

    if(!GetDisk(256000)) {
        printf("We are a bit tight in here need more disk.\n");
    }

    if(!CheckCPUID(0x756e6547)) {
        printf("Not an Intel CPU.\n");
    }

    return 0;
}
