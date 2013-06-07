#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <memory.h>

int input[1111];

int main(int argc, char **argv){
	if (argc % 2 == 0){
		return 1;
	}
	int num = argc;
	struct pollfd fds[num-1];
	int ret;
	for (int i = 1; i < num; i+=2){
    	fds[i - 1].fd = atoi(argv[i]);
    	fds[i - 1].events = POLLIN;
    	fds[i].fd = atoi(argv[i+1]);
    	fds[i].events = POLLOUT;
    }
    char **buffer = (char**)malloc((num / 2) * sizeof(char*));
    for (int i = 0; i < num / 2; i++){
    	buffer[i] = (char*)malloc(4096);
    }
    
    bool f[num];
    int life = argc / 2;
    for (int i = 0; i < num; i++) f[i] = 0;
    while (1){
    	ret = poll(fds, num - 1, -1);
    	if (ret == -1){
       		perror("poll");
       		return 1;
    	}
   		for (int i = 0; i < num - 1; i += 2){
   			int er = f[i];
   			if (fds[i].revents & POLLIN){
   				if (input[i] < 4096 && !f[i]){
   					int k = read(fds[i].fd, buffer[i / 2], 4096 - input[i]);
   					if (k == - 1){
   						f[i] = 1;
   					}else input[i] = input[i] + k;
   				}
   				if (input[i] == 0) f[i] = 1;
   			}
   			if (fds[i + 1].revents & POLLOUT){
   				if (input[i] != 0){
   					int k = write(fds[i + 1].fd, buffer[i / 2], input[i]);
   					if (k == -1){
   						f[i] = 1;
   					}else input[i] = input[i] - k;
   				}
   				fds[i + 1].events = fds[i + 1].events | POLLOUT; 
   			}
   			if (er == 0 && f[i] == 1){ 
   				life--;
   				fds[i].events = 0;
   				fds[i + 1].events = 0;	
   			}

   		}
   		if (!life) break;
   	}
   	free(buffer);
   	return 0;
}