#include <Windows.h>
#include <stdio.h>

VOID ResolveString(CHAR *data) {
    char charset[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-";
    int i = 0;
    int j = 0;
    for(i = 0; i < strlen(data); i++) {
        for(j = 0; j < strlen(charset); j++) {
            if(data[i] == charset[j]) {
                printf("%d,", j);
            }
        }
    }
}


int main(int argc, char **argv) {
    printf("Converting %s length is: %d\n", argv[1], strlen(argv[1]));
    ResolveString(argv[1]);
    return 0;
}
