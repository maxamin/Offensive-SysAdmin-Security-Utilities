#include <windows.h>

void dummy() {
    asm(".byte 0xcc, 0xcc");
}

int main() {
    CHAR *shellcode = dummy;

    EnumDesktopsW(NULL, (DESKTOPENUMPROCW)shellcode, NULL);

    return 0;
}
