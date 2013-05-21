#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <deque>
#include <vector>

void one_run(std::deque <char*> & v){
    char ** command = (char **) malloc((v.size() + 1) * (sizeof(char *)));
    for (size_t i = 0; i < v.size(); i++){
        command[i] = &v[i][0];
    }
    command[v.size()] = NULL;
    int pid = fork();
    if (pid){
        pid_t tpid;
        int status;
        do{
            tpid = wait(&status);
        } while (tpid != pid);
        free(command);
        _exit(0);
    }
    else {
        if (execvp(&v[0][0], command) == -1){
            _exit(1);
        }
    }
}


void run_all(std::deque<std::deque<char*> > & v){

    if (v.size() < 1){
        _exit(1);
    }
    if (v.size() == 1){
        one_run(v[0]);
    }
    else{
        int pipefd[2];
        pipe(pipefd);

        if (fork()){
            dup2(pipefd[0], 0);
            close(pipefd[0]);
            close(pipefd[1]);
            v.pop_front();
            run_all(v);
        }
        else{
            dup2(pipefd[1], 1);
            close(pipefd[0]);
            close(pipefd[1]);
            one_run(v[0]);
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        _exit(1);
    }
    int i, k = 100, flag, len = 0, input, er, m;
    int file = open(argv[1], O_RDONLY);
    char *buffer = (char*)malloc(k);
    std::deque<char*> v;
    std::deque <std::deque <char*> > run_v;
    while(1){
        input = read(file, buffer + len, k - len);
        if (input > 0){
            len = len + input;
        }
        if (len == 0 && input == 0){
            break;
        }
        m = -1;
        for (i = 0; i < len; i++) if (buffer[i] == '\0'){
            if (er == 1){
                er = 0;
                break;
            }
            char* c  = (char*)malloc(i - m);
            memcpy(c, buffer + m + 1, i - m - 1);
            c[i - m - 1] = '\0';
            v.push_back(c);
            //free(c);
            if (buffer[i + 1] == '\0') {
                flag = 1;
                m = i + 1;
                break;
            }
            m = i;
        }
        if (m == -1 && len == k){
            len = 0;
            er = 1;
        }else{
            len = len - m - 1;
            memmove(buffer, buffer + m + 1, len);
        }
        if (flag == 1){
            run_v.push_back(v);
            v.clear();

        }
    }
    run_all(run_v);
    free(buffer);
    return 0;
}

