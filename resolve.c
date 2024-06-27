#include <Windows.h>

int main(int argc, char** argv) {
    printf("Solving %s!%s\n", argv[1], argv[2]);
    printf("Address: 0x%p\n", GetProcAddress(LoadLibrary(argv[1]), argv[2]));
    return 0;
}
