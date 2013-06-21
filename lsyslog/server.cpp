#include <iostream>
#include <pty.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stropts.h>
#include <poll.h>
#include <errno.h>
#include <chrono>
#include <ctime>
#include <mutex>
#include <fcntl.h>
#define MAX_SIZE 4096

pid_t pid;
std::mutex mutex;

void handler(int){
    kill(pid, SIGINT);
}

void write_s(int fd, char* buffer, size_t length, pid_t pd){
    
    struct flock fl;
    fl.l_type = F_WRLCK; 
    fl.l_whence =  SEEK_SET;               
    fl.l_start = 0;   /* Starting offset for lock */
    fl.l_len = 1;     /* Number of bytes to lock */
    fl.l_pid = pd;     /* PID of process blocking our lock */
    fcntl(fd, F_SETLK, &fl);
    size_t output = 0;
    while (output < length){
        int r = write(fd, buffer + output, length - output);
        output += r;
        if (r < 0){
            exit(0);
        }
    }
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    
}

int main(){
    
    pid = fork();
    if (pid){
        signal(SIGINT, handler);
        std::cout << "Server starter with pid = " << pid << std::endl;
        int status;
            waitpid(pid, &status, 0);
        return 0;
    }
    int outfd = open("input.txt", O_CREAT | O_RDWR, S_IRWXU);
    setsid();
    addrinfo hints;
    
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    addrinfo *result;
    if (getaddrinfo(nullptr, "8442", &hints, &result) != 0){
        std::exit(EXIT_FAILURE);
    }
    if (result == nullptr){
        std::exit(EXIT_FAILURE);
    }

    int socket_fd;
    socket_fd = socket(result->ai_family, result->ai_socktype,
                        result->ai_protocol);
    if (socket_fd == -1){
        std::exit(EXIT_FAILURE);
    }

    int sso_status = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
                &sso_status, sizeof(int)) == -1){
        std::exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, result->ai_addr, result->ai_addrlen) == -1){
        std::exit(EXIT_FAILURE);
    }
    
    if (listen(socket_fd, 5) == -1){
        std::exit(EXIT_FAILURE);
    }

    while (true){
        int fd_acc = accept(socket_fd, result->ai_addr, &result->ai_addrlen);
        if (fd_acc == -1){
            std::exit(EXIT_FAILURE);
        }
        pid_t pd;
        if (pd = fork()){
            if (close(fd_acc) == -1){
                std::exit(EXIT_FAILURE);
            }
            continue;
        }
        
             char * buffer = (char *) malloc(MAX_SIZE);
                if (buffer == NULL) {
                    exit(0);
                }
                while (1) {
                    int input = read(fd_acc, buffer, MAX_SIZE);
                    if (input < 0){
                        break;
                    }
                    if (input == 0) 
                        break;
                    
                    write_s(outfd, buffer, input, pd); 
                    
                }
        
        close(socket_fd);
    }
}       

