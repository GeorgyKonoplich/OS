#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <deque>
#include <vector>
#include <iostream>

#define STDIN 0
#define STDOUT 1

const int BLOCK_SIZE = 1024;

int main(int argc, char* argv[]){
	char *buf = (char*)malloc(BLOCK_SIZE);
	for (int i = 1; i < argc; i++){
		int fd = atoi(argv[i]);
		while (1){
			int read_res = read(fd, buf, BLOCK_SIZE);
			if (read_res < 0){
				_exit(EXIT_FAILURE);
			}else if (read_res == 0){
				break;
			}
			int write_start = 0;
        	while (write_start < read_res){
            	int write_res = write(STDOUT, buf + write_start, read_res - write_start);
            	if (write_res == -1){
                	_exit(EXIT_FAILURE);
            	}
            	write_start += write_res;
        	}
		}
	}
}