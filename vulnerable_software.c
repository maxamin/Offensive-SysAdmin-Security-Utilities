#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
        char filename[] = "exploit.txt";
        char output[256];
        struct stat st;
        int size = 0;
        stat(filename, &st);
        size = st.st_size;

        int fd = open(filename, O_RDONLY);

        read(fd, output, size);
        close(fd);
        printf("%s\n", output);
        return 0;
}
