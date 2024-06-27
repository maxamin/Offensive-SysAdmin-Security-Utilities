#include <windows.h>
#include <stdio.h>
#include <winternl.h>

FARPROC DirectLdrLoadDll = NULL;
FARPROC DirectLoadLibraryA = NULL;
FARPROC DirectRtlInitUnicodeString = NULL;
FARPROC DirectGetProcAddress = NULL;

HANDLE LdrLoadDll(WCHAR *path) {
    HANDLE handle = NULL;
    UNICODE_STRING unicodePath;
    DirectRtlInitUnicodeString(&unicodePath, path);
    printf("%ls\n", unicodePath.Buffer);

    NTSTATUS status = DirectLdrLoadDll(NULL, 0, &unicodePath, &handle);
    return handle;
}

PEB *GetPEB() {
    /*
   0:   48 31 c0                xor    rax,rax
   3:   48 89 c3                mov    rbx,rax
   6:   80 c3 62                add    bl,0x62
   9:   48 83 eb 02             sub    rbx,0x2
   d:   65 48 8b 04 18          mov    rax,QWORD PTR gs:[rax+rbx*1]
  12:   c3                      ret
    */
    asm(".byte 0x48, 0x31, 0xc0, 0x48, 0x89, 0xc3, 0x80, 0xc3, 0x62, 0x48, 0x83, 0xeb, 0x02, 0x65, 0x48, 0x8b, 0x04, 0x18, 0xc3");
}

DWORD HashString(CHAR *buffer, DWORD dwSize) {
    DWORD hash = 0;
    DWORD i = 0;
    for(i; i < dwSize; i++) {
        hash += buffer[i];
        hash <<= 3;
        hash += buffer[i];
    }
    printf("%ls hash is 0x%08x\n", buffer, hash);

    return hash;
}

VOID *FindModuleBase(WCHAR *name) {
    PEB *peb = GetPEB();
    PEB_LDR_DATA *ldr = peb->Ldr;
    LIST_ENTRY *head = &ldr->InMemoryOrderModuleList;
    LIST_ENTRY *le = head->Flink;
    LDR_DATA_TABLE_ENTRY *dte = (LDR_DATA_TABLE_ENTRY*)le;

    do {
        /*
        DWORD hash = HashString(dte->FullDllName.Buffer, dte->FullDllName.Length);
        if(hash == argument) {
            you found it
        }
        */
        //printf("%ls\n", dte->FullDllName.Buffer);
        
        if(wcsicmp(dte->FullDllName.Buffer, name) == 0) {
            BYTE* addr = (BYTE*)dte;
            addr += 0x20;
            DWORD64 *deref = (DWORD64*)addr;
            DWORD64 final = *deref;
            printf("%ls base at 0x%016llx\n", name, final);
            return (VOID*)final;
        }
        le = le->Flink;
        dte = (LDR_DATA_TABLE_ENTRY*)le;
    } while(le != head);
}

FARPROC FindFunctionAddress(VOID* base, CHAR* functionName) {
    IMAGE_DOS_HEADER *MZ = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS *PE = (IMAGE_NT_HEADERS*)((BYTE*)base + MZ->e_lfanew);
    IMAGE_EXPORT_DIRECTORY *export = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)base + PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD *nameOffset = (DWORD*)((BYTE*)base + export->AddressOfNames);
    DWORD *functionOffset = (DWORD*)((BYTE*)base + export->AddressOfFunctions);
    WORD *ordinalOffset = (WORD*)((BYTE*)base + export->AddressOfNameOrdinals);

    DWORD i = 0;
    for(i; i < export->NumberOfNames; i++) {
        if(strcmp(functionName, (CHAR*)base + nameOffset[i]) == 0) {
            return (FARPROC)((BYTE*)base + functionOffset[ordinalOffset[i]]);
        }
    }
}

FARPROC Resolve(WCHAR* dll, CHAR *func) {
    HANDLE hLib = LdrLoadDll(dll);
    FARPROC ptr = DirectGetProcAddress(hLib, func);
    printf("%ls!%s at 0x%p\n", dll, func, ptr);
    return ptr;
}

int main() {
    VOID *base_nt = FindModuleBase(L"ntdll.dll");
    VOID *base_k32 = FindModuleBase(L"kernel32.dll");
    DirectLdrLoadDll = FindFunctionAddress(base_nt, "LdrLoadDll");
    DirectLoadLibraryA = FindFunctionAddress(base_k32, "LoadLibraryA");
    DirectGetProcAddress = FindFunctionAddress(base_k32, "GetProcAddress");
    DirectLdrLoadDll = FindFunctionAddress(base_nt, "LdrLoadDll");
    DirectRtlInitUnicodeString = FindFunctionAddress(DirectLoadLibraryA("ntoskrnl.exe"), "RtlInitUnicodeString");

    FARPROC DirectBind = Resolve(L"ws2_32.dll", "bind");

    return 0;
}
