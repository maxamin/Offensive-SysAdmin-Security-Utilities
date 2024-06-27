#include <windows.h>
#include <stdio.h>

#define MIN_COUNT 10

CHAR current[256];

int main() {
	DWORD passed = 0;
	memset(current, 0x00, 256);
	
	while(passed < MIN_COUNT) {
		HWND hwnd = GetForegroundWindow();
		CHAR *title = (CHAR*)GlobalAlloc(GPTR, 256);
		GetWindowTextA(hwnd, title, 256);
		if(strcmp(title, current) == 0) {
			strncpy(current, title, 256);
			passed++;
		}
		GlobalFree(title);
	}
	
	// You passed the user interaction check at this point code execute malicious code

	return 0;
}
