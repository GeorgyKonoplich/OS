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

int socket_fd;
const char * ident;
int option;
int facility;


char * getstring(long long x) {
    char * tmp = (char * ) malloc(1024);
    if (tmp == NULL) {
        perror("Alocate error\n");
        exit(6);
    }
    int k = 0;
    while(x > 0) {
        tmp[k] = '0' + (x % 10);
        x = x/10;
        k++;
    }
    char * ans = (char *) malloc((k + 1) * sizeof(char));
    if (ans == NULL) {
        perror("Alocating error\n");
        exit(6);
    }
    for (int i = 0; i < k; i++) {
        ans[i] = tmp[k - i - 1];
    }
    ans[k] = '\0';
    free(tmp);
    return ans;
}

void openlog(const char *_ident, int _option, int _facility) {
    ident = _ident;
    option = _option;
    facility = _facility;

    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    addrinfo *result;
    if (getaddrinfo(nullptr, "8442", &hints, &result) != 0){
        std::exit(EXIT_FAILURE);
    }
    if (result == nullptr){
        std::exit(EXIT_FAILURE);
    }
    socket_fd = socket(result->ai_family, result->ai_socktype,
                        result->ai_protocol);
    connect(socket_fd, result->ai_addr, result->ai_addrlen);
}

void write_s(int fd, const char* buffer, size_t length){
    size_t output = 0;
    while (output < length){
        int r = write(fd, buffer + output, length - output);
        output += r;
        if (r < 0){
            exit(0);
        }
    }
}

void syslog(int priority, const char *format){
    write_s(socket_fd, ident, strlen(ident));
    write_s(socket_fd, format, strlen(format));
    write_s(socket_fd, " ", 1);
    char * prior = getstring(priority);
    write_s(socket_fd, prior, strlen(prior));
    write_s(socket_fd, "\n", 1);
    char * opti = getstring(option);
    write_s(socket_fd, opti, strlen(opti));
    write_s(socket_fd, "\n", 1);
    char * fac = getstring(facility);
    write_s(socket_fd, fac, strlen(fac));
    write_s(socket_fd, "\n", 1);
}

