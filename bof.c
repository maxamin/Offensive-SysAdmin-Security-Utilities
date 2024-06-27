#include <Windows.h>
#include "beacon.h"

#define printf(format, args...) { BeaconPrintf(CALLBACK_OUTPUT, format, ## args); }

DECLSPEC_IMPORT FARPROC WINAPI kernel32$GetProcAddress(HANDLE, CHAR*);
DECLSPEC_IMPORT HANDLE WINAPI kernel32$LoadLibraryA(CHAR*);

FARPROC Resolver(CHAR *lib, CHAR *func) {
    FARPROC ptr = kernel32$GetProcAddress(kernel32$LoadLibraryA(lib), func);
    printf("%s$%s located at 0x%p\n", lib, func, ptr);
    return ptr;
}

int go(char *args, int length) {
    datap parser;

    BeaconDataParse(&parser, args, length);
    CHAR *name = BeaconDataExtract(&parser, NULL);

    FARPROC GetCurrentProcessId = Resolver("kernel32.dll", "GetCurrentProcessId");
    printf("hello %s your PID is %d", name, GetCurrentProcessId());
    return 0;
}
