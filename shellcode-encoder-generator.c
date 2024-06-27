#include <Windows.h>
#include <stdio.h>

// shellcode
// plaintext AAAABBBBCCCCDDDD
// key       ABCDABCDABCDABCD
// ciphertext ZZZZYYYYXXXXWWWW
// decoderassembly+ZZZZYYYYXXXXWWWW

int main(int argc, char**argv) {
    DWORD key = atoi(argv[1]);
    DWORD dwSize = 0;
    DWORD dwRead = 0;
    CHAR *shellcode = NULL;
    CHAR *original = NULL;
    DWORD i = 0;
    DWORD *current;
    HANDLE hFile = CreateFile(argv[2], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open %s\n", argv[2]);
        ExitProcess(1);
    }

    dwSize = GetFileSize(hFile, NULL);
    printf("Shellcode size is %d bytes.\n", dwSize);

	DWORD dwPadding = dwSize % 4;
	if(dwPadding != 0) {
		dwPadding = 4 - dwPadding;
		printf("Payload will be padded with %d bytes\n", dwPadding);
	}

    shellcode = GlobalAlloc(GPTR, dwSize + dwPadding);
    original = shellcode;
    ReadFile(hFile, shellcode, dwSize, &dwRead, NULL);

    printf("dwSize: %d\n", dwSize + dwPadding);

    for(i = 0; i < dwPadding; i++) {
        shellcode[dwSize + i] = 0x90;
    }


    for(i = 0; i <= (dwSize + dwPadding) / 4; i++) {
        current = (DWORD*)shellcode;
        *current = *current ^ key;
        shellcode += 4;
    }

    printf("Encoded using the following key 0x%08x (%d)\n", key, key);
    for(i = 0; i < dwSize + dwPadding; i++) {
        printf("0x%02x,", (unsigned char)original[i]);
    }
    printf("\n");

    for(i = 0; i < dwSize + dwPadding; i++) {
        printf("\\x%02x", (unsigned char)original[i]);
    }

    CloseHandle(hFile);
    GlobalFree(original);

    return 0;
}
