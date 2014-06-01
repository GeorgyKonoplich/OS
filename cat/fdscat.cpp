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

void write_reverse(char * dop_buffer, int kol, char * buffer, int start, int end){
	char *buf = (char*)malloc(end - start + 1);
	int len = 0;
	for (int i = end; i >= start; i--){
		buf[len] = buffer[i]; 
		len++;
	}
	int write_start = 0;
    while (write_start < len){
    	int write_res = write(STDOUT, buf + write_start, len - write_start);
        if (write_res == -1){
        	_exit(EXIT_FAILURE);
        }
        write_start += write_res;
    }
    free(buf);
    char *buf = (char*)malloc(kol);
    int len == 0;
    for (int i = kol; i >= 0; i--){
		buf[len] = dop_buffer[i]; 
		len++;
	}
	int write_start = 0;
    while (write_start < len){
    	int write_res = write(STDOUT, buf + write_start, len - write_start);
        if (write_res == -1){
        	_exit(EXIT_FAILURE);
        }
        write_start += write_res;
    }
}

int main(int argc, char* argv[]){
	char *buf = (char*)malloc(BLOCK_SIZE);
	char *dop_buf = (char*)malloc(BLOCK_SIZE);
	for (int i = 1; i < argc; i++){
		int fd = atoi(argv[i]);
		int len = 0;
		int kol = 0;
		int len1 = 0;
		while (1){
			int read_res = read(fd, buf + len, BLOCK_SIZE - len);
			if (read_res >= 0) len = len + read_res;
			if (read_res < 0){
				_exit(EXIT_FAILURE);
			}else if (read_res == 0 && len != 0){
				buf[len] = '\n';
				len = len + 1;
			}else if (read_res == 0){
				break;
			}
			int m = 0;
			for (int j = 0; j < len; j++) if (buf[j] == '\n'){
				if (kol == 0){
					len1 = j - m + 1;
					int loop = 0;
					for (int k = m; k <= j; k++){ 
						dop_buf[loop] = buf[k];
						loop++;
					}
					kol++;  
				}
				if (kol == 1){
					write_reverse(dop_buf, len1, buf, m, j);
					kol = 0;
				}
				m = j + 1;
			}
			len = len - m;
			memmove(buf, buf + m , len);
		}
		close(fd);
	}
}