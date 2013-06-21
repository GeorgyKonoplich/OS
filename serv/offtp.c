#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
using namespace std;

const int MAX_SIZE = 4096;
int pid;

void handler(int) {
    if (pid)
        kill(pid, SIGINT);
}

void write_to_serv(int fd, char * buffer, int len) {
    int output = 0;
    while (output < len) {
        output += write(fd, buffer + output, len - output);
    }
}

int main() {
    if ((pid = fork())) {
        signal(SIGINT, &handler); 
        int status;
        waitpid(pid, &status, 0);
        return 0;   
    } else {
        int status, sd, new_sd;
        struct addrinfo hints, *res;
        struct sockaddr_storage sock_stor;
        setsid();

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;    
        hints.ai_socktype = SOCK_STREAM;    
        hints.ai_flags = AI_PASSIVE;   

        if ((status = getaddrinfo(NULL, "8822", &hints, &res)) != 0) {
            perror("error getaddrfinfo");
            return 1;
        } 
        if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("error socket");
            return 2;
        } 

        status = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(int)) == -1) {
            close(sd);
            perror("setsockopt fail");
            return 3;
        }

        if (bind(sd, res->ai_addr, res->ai_addrlen) == -1) {
            close(sd);
            perror("bind fail");
            return 4;
        }
        
        if (listen(sd, 5) == -1) {
            close(sd);
            perror("listen fail");
            return 5;
        }
        while (1) {
            socklen_t addr_size = sizeof(sock_stor);
            if ((new_sd = accept(sd, (struct sockaddr *) &sock_stor, &addr_size)) == -1) {
                close(sd);
                perror("accept fail");
                return 6;
            }
            
            if (!fork()) {
                close(sd);
                char msg [] = "Hello\n";
                write_to_serv(new_sd, msg, 6);
                auto start = chrono::high_resolution_clock::now();
                int input = 0;
                int buffer_size = MAX_SIZE;
                char *buffer = (char*)malloc(buffer_size);
                if (buffer == NULL){
                    exit(0);
                }
                /*while (true){
                    int k = read(new_sd, buffer + input, buffer_size - input);
                    input += k;
                    if (k == 0)
                        break;
                    if (k < 0){
                        exit(0);
                    }
                }*/
                auto stop = chrono::high_resolution_clock::now();
                double timer = chrono::duration<double>(stop - start).count();
                cout << timer << endl;
            }
            close(new_sd);
        }
        
        close(sd);
        return 0;

    }
}
