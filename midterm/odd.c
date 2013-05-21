#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

struct mypair{
    char* data;
    size_t len;
};

void _print(struct mypair q){
    size_t ans = 0;
    int output;
    while (ans < q.len){
        output = write(1, q.data + ans, q.len - ans);
        if (output < 0){
            printf("ERROR");
            exit(0);
        }
        ans = ans + output;
    }
}

struct mypair drop_even(char* buf, size_t len){
    size_t j;
    int len1;
    int kol;
    kol = 0;
    if (len % 2 == 0) len1 = len / 2; else len1 = len / 2 + 1;
    struct mypair tmp;
    tmp.data = (char* )malloc(len1 + 1);
    for (j = 0; j < len; j++) if (j % 2 == 0){
        *(tmp.data + kol) = buf[j];
        kol++;
    } 
    *(tmp.data + kol) = '\0';
    kol++;
    tmp.len = kol;
    return tmp;
}

int main(int argc, char* argv[]){
    int i;
    int k = 1000;
    int len = 0;
    int input, m, er;
    char *buffer = (char*)malloc(k + 1);
    while (1){
        input = read(0, buffer + len, k - len);
        if (input > 0){
            len = len + input;
        }
        if (len == 0 && input == 0){
            break;
        } else if (input == 0){
            buffer[len] = '\n';
            len = len + 1;
        }
        m = -1;
        for (i = 0; i < len; i++) if (buffer[i] == '\n'){
            if (er == 1){
                er = 0;
                m = i;
                break;
            }
            char* c = (char*)malloc(i - m - 1);
            memcpy(c, buffer + m + 1, i - m - 1);
            struct mypair b;
            b = drop_even(c, i - m - 1);
            for (k = 1; k < argc; k++){
                int pid = fork();
                int status;
                int length = strlen(argv[k]);
                char** args = malloc((1 + b.len + length)*sizeof(char*));
                args[1] = b.data;
                args[2] = '\0';
                args[0] = argv[k];
                if (pid == 0){
                    int fd = open("dev/null", O_WRONLY);
                    dup2(fd, 1);
                    dup2(fd, 2);
                    execvp(args[0], args);
                }
                wait(&status);
                //if (status == 0) printf("good\n");
                free(args);
            }
            //_print(b);
            free(c);
            free(b.data);
            m = i;
        }
        if (m == -1 && len == k){
            len = 0;
            er = 1;
        }else{
            len = len - m  - 1;
            memmove(buffer, buffer + m + 1, len);
        }
    }
    free(buffer);
    return 0;
}
