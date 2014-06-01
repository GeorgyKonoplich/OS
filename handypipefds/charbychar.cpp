#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <memory.h>

const int BUFFER_SIZE = 4096;

int main(int argc, char** argv){
	for (int i = 1; i < argc; i+=2){
		int kol = atoi(argv[i]);
		int sleep_time = atoi(argv[i + 1]);
		int count_size = 0;
		char* buf = (char*)malloc(BUFFER_SIZE);
		while (count_size < kol){
			int result = read(0, buf + count_size, kol - count_size);
			if (result == -1){
				_exit(EXIT_FAILURE);
			}
			count_size += result;
		}
		int write_size = 0;
		while (write_size < kol){
			int result = write(1, buf + write_size, kol - write_size);
			if (result == -1){
				_exit(EXIT_FAILURE);
			}
			write_size += result;
		}
		sleep(sleep_time);
	}
	while(true){
		int kol = 1;
		int sleep_time = 0;
		int count_size = 0;
		char* buf = (char*)malloc(BUFFER_SIZE);
		while (count_size < kol){
			int result = read(0, buf + count_size, kol - count_size);
			if (result == -1){
				_exit(EXIT_FAILURE);
			}
			count_size += result;
		}
		int write_size = 0;
		while (write_size < kol){
			int result = write(1, buf + write_size, kol - write_size);
			if (result == -1){
				_exit(EXIT_FAILURE);
			}
			write_size += result;
		}
		sleep(sleep_time);	
	}
	return 1;
}