#include <Windows.h>
#include <stdio.h>

int main() {
    FARPROC address = GetProcAddress(LoadLibrary("User32.dll"), "MessageBoxA");
    printf("0x%p\n", address);

    int(*MyMessageBox)(HWND, LPCTSTR, LPCTSTR, UINT);
    MyMessageBox = address;
    MyMessageBox(NULL, "test", "test", MB_OK);

    return 0;
}

