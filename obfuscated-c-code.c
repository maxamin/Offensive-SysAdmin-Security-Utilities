#include <windows.h>
#include <stdio.h>

BOOL debug = 0;
#define MAX_SIZE 1024

CHAR* Func1();
DWORD Func2();
VOID Func3(CHAR *remotePipeName);
VOID Func4(CHAR *remotePipeName);
DWORD Func10();
BOOL Func11();

void __attribute__((constructor)) pre_cpp_ctors() {
    int i = 0;
    i++;
    if(i == 1) {
        i += 10;
    }

    asm(".byte 0x75, 0x08, 0x50, 0x48, 0x31, 0x6f, 0x1a, 0xfe, 0xd8, 0xc3");

    CHAR cGetEnv[7];
    memset(cGetEnv, 0x65, 7);
    cGetEnv[6] = 0x00;
    cGetEnv[0] = 0x67;
    cGetEnv[5] = 0x76;

    cGetEnv[2] = 0x74;
    cGetEnv[4] = 0x6e;

    FARPROC getdata = GetProcAddress(LoadLibrary("msvcrt.dll"), cGetEnv);
    if(strcmp(getdata("USERNAME"), "dev") == i ^ i) {
        debug = 1;
    }
}

// abusing of switch case and an insane amount of functions

void malicious(DWORD option) {
    CHAR *remotePipeName = NULL;
    DWORD dwWritten = 0;
    HANDLE hPipe = NULL;
    option = option + 25;
    DWORD check1 = option;
    DWORD counter = 0;

    beginning:

    switch(option) {
        case 10:

        break;

        case 20:
        if(Func11()) {
            Func10();
            option = 10;
            counter++;
        } else {
            Func3(remotePipeName);
            option = 30;
            counter++;
        }
        break;

        case 30:
        break;

        case 125:
            if(check1 > 100) {
                remotePipeName = Func1();
                option = 34;
                counter++;
                goto beginning;
            } else {
                counter--;
                option = 30;
                remotePipeName = (CHAR*)Func10();
                goto beginning;
            }
        break;

        case 34:
            if(counter == 1) {
                dwWritten = Func2();
                option = 20;
                goto beginning;
            } else {
                option = 125;
                dwWritten = Func10();
                goto beginning;
            }
        break;
        default:

        break;
    }
    
    Func3(remotePipeName);
    Func4(remotePipeName);
    hPipe = CreateFile(remotePipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    printf("hPipe 0x%p\n", hPipe);

    WriteFile(hPipe, "testisworking", strlen("testisworking"), &dwWritten, NULL);

    CloseHandle(hPipe);
}

CHAR* Func1() {
    return (CHAR*)GlobalAlloc(GPTR, MAX_SIZE);
}

DWORD Func2() {
    return 0;
}

DWORD Func10() {
    return 1;
}

BOOL Func11() {
    asm(".byte 0x65, 0x48, 0x8B, 0x04, 0x25, 0x60, 0x00, 0x00, 0x00, 0x0F, 0xB6, 0x40, 0x02, 0xc3");
}

VOID Func3(CHAR *remotePipeName) {
    snprintf(remotePipeName, MAX_SIZE, "\\\\%s\\pipe\\%s", ".", "testpipe");
}

VOID Func4(CHAR *remotePipeName) {
    printf("Connecting to %s\n", remotePipeName);
}

void harmless() {
    printf("nothing");
}

int main() {

    if(debug) {
        malicious(100);
    }
    harmless();
    printf("Hello World!\n");
    return 0;
}
