/*
import address table 
export address table
*/

#include <Windows.h>
#include <winnt.h>
#include <stdio.h>

int main(int argc, char **argv) {
    PCHAR dllname = argv[1];
    HANDLE hDll = LoadLibrary(dllname);
    printf("%s is loaded at 0x%p\n", dllname, hDll);

    PIMAGE_DOS_HEADER MZ = (PIMAGE_DOS_HEADER)hDll;
    PIMAGE_NT_HEADERS PE = (PIMAGE_NT_HEADERS)((LPBYTE)MZ + MZ->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY export = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)MZ + PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    PDWORD exportName = (PDWORD)((LPBYTE)MZ + export->AddressOfNames);
    
    DWORD i = 0;
    for(i; i < export->NumberOfNames; i++) {
        printf("%s\n", (PVOID)MZ + exportName[i]);
    }

    return 0;
}
