#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#define STDIN 0
#define STDOUT 1

const int BLOCK_SIZE = 1024;


void write_s(char * buf){
	int len = strlen(buf);
	int write_start = 0;
    while (write_start < len){
    	int write_res = write(STDOUT, buf + write_start, len - write_start);
        if (write_res == -1){
        	_exit(EXIT_FAILURE);
        }
        write_start += write_res;
    }

}
void parse_file(char* buf, int start, int end){
	int times = 0;
	int uid_, modes;
	for (int i = start; i < end; i++) if (buf[i] == ' '){
		if (times == 0){
			char* number = (char*)malloc(i - start);
			memcpy(number, buf + start, i - start);
			uid_ = atoi(number);
			free(number);
			times++;
		}else if (times == 1){
			modes = i - 1;	
			break;
		}
	}
	if (seteuid(uid_) < 0){
		_exit(EXIT_FAILURE);
	}
	char* buffer = (char*)malloc(end - modes);
	memcpy(buffer, buf + modes + 2, end - modes -2);
	int fd;
	if (buf[modes - 1] != ' '){
		fd = open(buffer, O_RDWR);
	}else if (buf[modes] == 'r'){
		fd = open(buffer, O_RDONLY);
	}else{
		fd = open(buffer, O_WRONLY);
	}
	free(buffer);
	if (fd < 0) write_s("no\n"); else write_s("yes\n");
	if (setuid(0) < 0){
		_exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv){
	if (getuid()){
		_exit(EXIT_FAILURE);
	}
	char* buf = (char*)malloc(BLOCK_SIZE);
	int len = 0;
	while(1){
		int input = read(STDIN, buf + len, BLOCK_SIZE - len);
		len = len + input;
		if (input < 0){
			_exit(EXIT_FAILURE);
		}
		if (input == 0 && len != 0){
			buf[len] = '\0';
			len = len + 1;
		}else if (input == 0){
			break;
		}
		int m = 0;
		for (int i = 0; i < len; i++) if (buf[i] == '\0'){
			parse_file(buf, m, i);
			m = i;
		}
		len = len - m;
		memmove(buf, buf + m, len);
	}
	return 0;
}
