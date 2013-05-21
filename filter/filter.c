#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){
    int length, res, k, input, m, i;
    char **args;
    int len = 0;
    char tmp = '\n';
    k = 4096;
    while ((res = getopt(argc, argv, "nzb:")) != -1) {
        switch (res) {
            case 'n':
                tmp = '\n';
                break;
            case 'z':
                tmp = '\0';
                break;
            case 'b':
                k = atoi(optarg);
                break;
        }
    }
    length = argc - optind;
    args = malloc(length + 2);
    int j;
    for (j = 0; j < len; j++) {
        args[j] = argv[j + optind];
    }
    length += 2;
    char* buf = (char*) malloc(k + 2);
    while(1){
        input = read(0, buf + len, k - len + 1);
        if (input >= 0) len =  len + input;
        if (input == 0 && len == 0){
            break;
        } else if (input == 0){
            buf[len] = tmp;
            len = len + 1;
        }
        m = -1;
        for (i = 0; i < len; i++) if (buf[i] == tmp){
            char *buffer = malloc((i + 1) * sizeof(char));
            memmove(buffer, buf + m + 1, i - m - 1);
            args[length - 2] = buffer;
            args[length - 1] = 0;
            int pid = fork();
            int status;
            if (pid == 0){
                int fd = open("/dev/null", O_WRONLY);
                dup2(fd, 1);
                dup2(fd, 2);
                execvp(args[0], args);
            }
            wait(&status);
            if (status == 0) printf("%s\n", args[length - 2]);
            free(buffer);
            m = i;
        }
        if (m == -1 && len == k + 1){
            break;
        }else{ 
            len = len - m - 1;
            memmove(buf, buf + m + 1, len);
        }
    }
    free(buf);
    return 0;
}
