#include <Windows.h>
#include <stdio.h>
#include <winternl.h>

/*
__kernel_entry NTSTATUS NtCreateFile(
  PHANDLE            FileHandle,
  ACCESS_MASK        DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK   IoStatusBlock,
  PLARGE_INTEGER     AllocationSize,
  ULONG              FileAttributes,
  ULONG              ShareAccess,
  ULONG              CreateDisposition,
  ULONG              CreateOptions,
  PVOID              EaBuffer,
  ULONG              EaLength
);

*/

/*
typedef struct _OBJECT_ATTRIBUTES {
  ULONG           Length;
  HANDLE          RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG           Attributes;
  PVOID           SecurityDescriptor;
  PVOID           SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
*/

NTSTATUS __attribute__ ((noinline)) SyscallNtCreateFile(
  PHANDLE            FileHandle,
  ACCESS_MASK        DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK   IoStatusBlock,
  PLARGE_INTEGER     AllocationSize,
  ULONG              FileAttributes,
  ULONG              ShareAccess,
  ULONG              CreateDisposition,
  ULONG              CreateOptions,
  PVOID              EaBuffer,
  ULONG              EaLength
) {

/*
ntdll.dll NtCreateFile
0000000000000000 <.text>:
   0:   49 89 ca                mov    r10,rcx
   3:   b8 55 00 00 00          mov    eax,0x55
   8:   0f 05                   syscall
   a:   c3                      ret
*/

    asm(".byte 0x49, 0x89, 0xca, 0xb8, 0x55, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc3");
}

int main() {
    FARPROC RtlInitUnicodeString = GetProcAddress(LoadLibrary("ntdll.dll"), "RtlInitUnicodeString");
    printf("RtlInitUnicodeString address 0x%p\n", RtlInitUnicodeString);
    HANDLE hFile = NULL;
    UNICODE_STRING pus;
    IO_STATUS_BLOCK isb = {0};
    LARGE_INTEGER li;
    li.QuadPart = 256;

    PCWSTR path = L"\\??\\C:\\Users\\public\\test.nt";
    RtlInitUnicodeString(&pus, path);

    OBJECT_ATTRIBUTES oa = {0};
    oa.Length = sizeof(OBJECT_ATTRIBUTES);
    oa.RootDirectory = NULL;
    oa.ObjectName = &pus;
    oa.Attributes = OBJ_CASE_INSENSITIVE;
    oa.SecurityDescriptor = NULL;
    oa.SecurityQualityOfService = NULL;

    // NtCreateFile arguments are going to be
    // hfile
    // STANDARD_RIGHTS_ALL
    // &oa
    // &isb
    // &li
    // FILE_ATTRIBUTE_NORMAL
    // FILE_SHARE_READ
    // FILE_CREATE
    // FILE_NON_DIRECTORY_FILE
    // NULL
    // NULL

    SyscallNtCreateFile(&hFile, STANDARD_RIGHTS_ALL, &oa, &isb, &li, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_CREATE, FILE_NON_DIRECTORY_FILE, NULL, NULL);

    // WriteFile(hFile, ...);
    printf("HANDLE VALUE 0x%p\n", hFile);

    return 0;
}
