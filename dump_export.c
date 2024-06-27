#include <windows.h>
#include <stdio.h>
#include <winnt.h>

VOID DumpExport(VOID *lib) {
    IMAGE_DOS_HEADER *MZ = (IMAGE_DOS_HEADER*)lib;
    IMAGE_NT_HEADERS *PE = (IMAGE_NT_HEADERS*)((BYTE*)lib + MZ->e_lfanew);
    IMAGE_EXPORT_DIRECTORY *export = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)lib + PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD *name = (DWORD*)((BYTE*)lib + export->AddressOfNames);

    DWORD i = 0;
    for(i = 0; i < export->NumberOfNames; i++) {
        printf("%s\n", (CHAR*)lib + name[i]);
    }
}

int main(int argc, char **argv) {
    CHAR *dllname = argv[1];
    HANDLE hDll = LoadLibrary(dllname);

    if(hDll == NULL) {
        printf("Failed to load\n");
        ExitProcess(0);
    }

    DumpExport(hDll);

    CloseHandle(hDll);

    return 0;
}
