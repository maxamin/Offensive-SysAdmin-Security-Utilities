#include <Windows.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if(argc < 2) {
        printf("Usage: %s offset data\n", argv[0]);
        ExitProcess(0);
    }

    CHAR *data = argv[2];
    DWORD offset = atoi(argv[1]);
    DWORD i = 0;

    for(i; i < strlen(data); i++) {
        CHAR current = data[i];
        printf("%c", current + offset);
    }
    printf("\n");
    return 0;
}
