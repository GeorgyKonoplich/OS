#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <memory.h>
#include <errno.h>

const int BLOCK_SIZE = 100;

int main(int argc, char** argv){
	if (argc % 2 == 0){
		_exit(EXIT_FAILURE);
	}
	argc = argc - 1;
	struct pollfd fds[argc], fds1[argc];

	for (int i = 0; i < argc; i += 2){
		fds[i].fd = atoi(argv[i + 1]);//open(argv[i + 1], O_RDONLY); atoi(argv[i + 1]);
    	fds1[i].fd = fds[i].fd;
    	fds[i].events = POLLIN;
    	fds[i + 1].fd = atoi(argv[i + 2]);//open(argv[i + 2], O_WRONLY); atoi(argv[i + 2]);
    	fds1[i + 1].fd = fds[i + 1].fd;
    	fds[i + 1].events = POLLOUT;
	}
	
	char **buffer = (char**)malloc((argc / 2) * sizeof(char*));
    for (int i = 0; i < argc / 2; i++){
    	buffer[i] = (char*)malloc(BLOCK_SIZE);
    }
	int input[argc];
	int life = argc;
	for (int i = 0; i < argc; i++) input[i] = 0;
	while(life){

		int ret = poll(fds, argc, -1);
		if (ret == -1 && errno != EINTR){
			_exit(EXIT_FAILURE);
		}
		for (int i = 0; i < argc; i += 2){
			
			if (fds[i].revents & (POLLIN | POLLHUP)) {
				int result = read(fds[i].fd, buffer[i / 2] + input[i], BLOCK_SIZE - input[i]);
				if (result == -1){
					fds[i].fd = -1;
					input[i] = 0;
				}
				if (result == 0){
					fds[i].fd = -1;
				}
				input[i] += result;
				fds[i].revents = 0;
			}
			if (fds[i + 1].revents & POLLOUT){
				if (input[i] == 0){
					if (fds[i].fd == -1) {
						fds[i + 1].fd = -1;
						life -= 2;
					}
				}else{
					int result = write(fds[i + 1].fd, buffer[i / 2], input[i]);
					if (result == -1){
						fds[i + 1].fd = -1;
						fds[i].fd = -1;
						life -= 2;
					}else{
						input[i] = input[i] - result;
						memmove(buffer[i / 2], buffer[i / 2] + result, input[i]);
					}
				}
				fds[i].revents = 0;
			}
			//if (input[i] == BLOCK_SIZE) fds[i].fd = -1; else fds[i].fd = fds1[i].fd;

			if (input[i] == 0) fds[i + 1].events = 0; else fds[i + 1].events = POLLOUT;
			if (input[i] == BLOCK_SIZE) fds[i].events = 0; else fds[i].events = POLLIN;
			//int tmp = 0;
			//for (int i = 0; i < argc; i++) if (fds[i].fd != -1) tmp++;
			//if (tmp == 0) break;
		}
	}
	return 1;
}
