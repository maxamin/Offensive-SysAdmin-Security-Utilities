#pragma once
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#if defined(_M_X64) || defined(__x86_64__)
    #include <hde/hde64.h>
    typedef hde64s HDE;
    #define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#else
    #include <hde/hde32.h>
    typedef hde32s HDE;
    #define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#endif

struct PatchBlocks { void* bytes; size_t size; };
static std::map<LPBYTE, PatchBlocks> patchesTable; // This is the map we insert into

VOID HookPatchTrampoline(const char* szModuleName, const char* szFuncName, LPVOID hookedFunctionAddress, FARPROC* lpOldFunc, BOOL patchDirect) {
    size_t instructionLength = 0;
#ifdef _M_IX86
    //x86 Hooker
#elif defined(_M_AMD64)
    char trampolineBytes[48];
    char originalBytes[16];
    char newBytes[14];
#endif 


    // load required DLL
    HINSTANCE library = LoadLibraryA(szModuleName);
    if (!library) {
        return;
    }

    // get address of the function in memory
    FARPROC funcAddress = GetProcAddress(library, szFuncName);
    if (!funcAddress) {
        return;
    }

#ifdef _M_IX86
    //X86 Hooker
#elif defined(_M_AMD64)   
    // Discover code cave
    // GB is 1 Billion Bytes.  We can go up to 2 Billion bytes forward and 2 Billion bytes backwards.
    // Instead we just find all executable locations and enumerate them by size.  Nothing will be 2gb in mem....unlikely at least.
    //https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_section_header
    PIMAGE_SECTION_HEADER Section = 0;
    HMODULE hMod = GetModuleHandleA(szModuleName);
    int i;
    LPBYTE currentByte = 0;
    size_t caveLength = 0;
    size_t totalSize = sizeof(newBytes) + sizeof(trampolineBytes);
    BOOL found = FALSE;
    // While a valid cave is not found keep searching binary sections
    while (found == FALSE) {
        if (hMod)
        {
            PIMAGE_NT_HEADERS64 NtHeader = ImageNtHeader(hMod);
            WORD NumSections = NtHeader->FileHeader.NumberOfSections;
            if (Section == 0) {
                Section = IMAGE_FIRST_SECTION(NtHeader);
            }
            for (WORD i = 0; i < NumSections; i++)
            {
                DWORD characteristics = Section->Characteristics;
                // Check if the memory is executable
                if (characteristics & IMAGE_SCN_MEM_EXECUTE) {
                    printf("Executable Region at: %-8s\t%x\t%x\t%x\n", Section->Name, Section->VirtualAddress,
                        Section->PointerToRawData, Section->SizeOfRawData);
                    break;
                }
                Section++; // If not executable check next section
            }
        }
        // Search each executable section for a cave
        for (i = 0; i < Section->SizeOfRawData; i += totalSize) {
            currentByte = (LPBYTE)library+Section->VirtualAddress+i;
            if (memcmp(currentByte, "\x00", 1) == 0) {
                caveLength += 1;
                LPBYTE newByteBackward = currentByte - 1;
                size_t caveLengthBackward = 0;
                if (memcmp(newByteBackward, "\x00", 1) == 0) {
                    while (memcmp(newByteBackward, "\x00", 1) == 0) {
                        caveLength++;
                        caveLengthBackward++;
                        newByteBackward--;
                    }
                }
                LPBYTE newByteForward = currentByte + 1;
                size_t caveLengthForward = 0;
                if (memcmp(newByteForward, "\x00", 1) == 0) {
                    while (memcmp(newByteForward, "\x00", 1) == 0) {
                        caveLength++;
                        caveLengthForward++;
                        newByteForward++;
                    }
                }
                if (caveLength >= totalSize) {
                    currentByte -= caveLengthBackward;
                    while (memcmp(currentByte - 1, "\x00", 1) != 0 || memcmp(currentByte - 2, "\x00", 1) != 0) {
                        currentByte++;
                    }
                    found = TRUE;
                    break;
                }
                else {
                    i += caveLengthForward;
                    caveLength = 0;
                    continue;
                }
            }
        }
        // If cave isn't found we go to the next section
        Section++;
    }
    // Get the relative difference between the location of the cave and the function we are hooking
    DWORD diff = (DWORD)currentByte - (DWORD)funcAddress - 5;
    printf("FOUND CAVE OF SIZE:%d for function: %s at location: %p\n", caveLength, szFuncName, currentByte);

    BOOL relativeJmp = FALSE; // If a code cave is found the relative jmp back is not required.
    BOOL relative = FALSE; // Same use
    // Read the first instruction into the trampoline
    HDE hs;
    HDE_DISASM((LPVOID)funcAddress, &hs);
    instructionLength = hs.len;
    ReadProcessMemory(GetCurrentProcess(), funcAddress, trampolineBytes, instructionLength, NULL);
    ReadProcessMemory(GetCurrentProcess(), funcAddress, originalBytes, instructionLength, NULL);
    if ((hs.modrm & 0xC7) == 0x05) {
        relative = TRUE;
        printf("Relative Detected\n");
        PUINT32 relativeAddress = (PUINT32)(trampolineBytes + hs.len - ((hs.flags & 0x3C) >> 2) - 4);
        *relativeAddress = (UINT32)(((ULONG_PTR)funcAddress + hs.len + (INT32)hs.disp.disp32) - ((ULONG_PTR)currentByte + hs.len + 14));

        if (hs.opcode == 0xFF && hs.modrm_reg == 4)
            relativeJmp = TRUE;
    }

    // While the total instruction length is less than 5 keep iterating, we need 5 bytes for our relative jmp
    while (instructionLength < 5) {
        LPBYTE newFuncAddress = LPBYTE(funcAddress) + instructionLength;
        HDE_DISASM((LPVOID)newFuncAddress, &hs);
        SIZE_T newBytesLength = hs.len;

        if ((hs.modrm & 0xC7) == 0x05) {
            relative = TRUE;
            printf("Relative Detected\n");
            PUINT32 relativeAddress = (PUINT32)(trampolineBytes + hs.len - ((hs.flags & 0x3C) >> 2) - 4);
            *relativeAddress = (UINT32)(((ULONG_PTR)funcAddress + hs.len + (INT32)hs.disp.disp32) - ((ULONG_PTR)currentByte + hs.len + 14 + instructionLength));
            if (hs.opcode == 0xFF && hs.modrm_reg == 4)
                relativeJmp = TRUE;
        }

        ReadProcessMemory(GetCurrentProcess(), newFuncAddress, (LPBYTE)((LPBYTE)trampolineBytes + instructionLength), newBytesLength, NULL);
        instructionLength += newBytesLength;
    }

    // Put the relative jump at the function location
    // Write jmp qword pter to our hooked function after
    unsigned char relJump[5];
    relJump[0] = 0xe9;
    memcpy_s(relJump +1, 4 ,(unsigned char*)&diff,4);
    memcpy_s(newBytes, 6, "\xFF\x25\x00\x00\x00\x00", 6);
    memcpy_s(newBytes + 6, 8, &hookedFunctionAddress, 8);
    WriteProcessMemory(GetCurrentProcess(), (LPBYTE)funcAddress, (LPCVOID)relJump, 5, NULL);
    WriteProcessMemory(GetCurrentProcess(), (LPBYTE)currentByte, newBytes, sizeof(newBytes), NULL);

    // A relative jmp doesnt require the additional jmp
    LPVOID offset;
    if (relativeJmp == TRUE) {
        memcpy(trampolineBytes + instructionLength, originalBytes, instructionLength);
        WriteProcessMemory(GetCurrentProcess(), (LPBYTE)currentByte + sizeof(newBytes), trampolineBytes, instructionLength * 2, NULL);
        offset = (LPVOID)((LPBYTE)currentByte + sizeof(newBytes) + instructionLength);
    }
    // Any other relative instruction requires the PUSH QWORD pointer again
    else if (relative == TRUE) {
        *(PDWORD64)(trampolineBytes + instructionLength) = 0x0000000025FF; // 
        *(PDWORD64)(trampolineBytes + instructionLength + 6) = (DWORD64)funcAddress + instructionLength;
        memcpy(trampolineBytes + instructionLength + 6 + 8, originalBytes, instructionLength);
        WriteProcessMemory(GetCurrentProcess(), (LPBYTE)currentByte + sizeof(newBytes), trampolineBytes, instructionLength * 2 + 6 + 8, NULL);
        offset = (LPVOID)((LPBYTE)currentByte + sizeof(newBytes) + instructionLength + 6 + 8);
    }
    else {
        // If there are no relatives then we add our trampoline + jmp to the cave
        *(PDWORD64)(trampolineBytes + instructionLength) = 0x0000000025FF; // 
        *(PDWORD64)(trampolineBytes + instructionLength + 6) = (DWORD64)funcAddress + instructionLength;
        WriteProcessMemory(GetCurrentProcess(), (LPBYTE)currentByte + sizeof(newBytes), trampolineBytes, instructionLength + 14, NULL);
        offset = (LPVOID)((LPBYTE)currentByte + sizeof(newBytes));
    }
    // Set the new location of the old function to the start of the trampoline
    *(PDWORD64)lpOldFunc = (DWORD64)((LPBYTE)currentByte + sizeof(newBytes));
    // Insert the patche into the map for restoration later
    patchesTable.insert(std::pair<LPBYTE, PatchBlocks>((LPBYTE)funcAddress, { offset, instructionLength }));
#endif
}

VOID UndoPatches() {
    std::map<LPBYTE, PatchBlocks>::iterator it;
    for (it = patchesTable.begin(); it != patchesTable.end(); ++it) {
        unsigned char bytes[16];
        *(PDWORD64)bytes = *(PDWORD64)it->second.bytes;
        // First write the old bytes back
        WriteProcessMemory(GetCurrentProcess(), (LPBYTE)it->first, (LPCVOID)bytes, it->second.size, NULL);

        // Then clear the cave data so as not to leave artifacts
        LPBYTE caveData = (LPBYTE)it->second.bytes-14;
        for (int i = 0; i < (it->second.size + 28); i++) {
            WriteProcessMemory(GetCurrentProcess(), caveData+i, "\x00", 1, NULL);
        }
    }
}