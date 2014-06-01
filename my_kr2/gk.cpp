#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define IN 0
#define OUT 1
#define BUF_SIZE 4096
using namespace std;

int check(int result){
	if (result < 0)
		_exit(1);
	return result;
}

void execute(char** argv, int fd_in, int fd_out){
	int pid = check(fork());
	if (pid){
		check(close(fd_in));
		check(close(fd_out));
	}else{
		check(dup2(fd_in, IN));
		check(dup2(fd_out, OUT));
		execvp(argv[0], argv);
		_exit(1);
	}
}

char** get_command(char* buf, int size, char sep){
	int kol = 0;
	for (int i = 0; i <= size; i++) if (buf[i] == sep || buf[i] == '\n'){
		kol++;
		buf[i] = '\0';
	}
	char** command = (char**)malloc(sizeof(char*) * (kol + 1));
	kol = 0;
	int separator = 0;
	for (int i = 0; i <= size; i++) if (buf[i] == '\0'){
		char* tmp = (char*)malloc(i - separator + 1);
		memcpy(tmp, buf + separator, i - separator + 1);
		command[kol] = tmp;
		separator = i + 1;
		kol++;
	}
	command[kol] = NULL;
	return command;
}

int main(int argc, char** argv){
	if (argc != 3){
		_exit(1);
	}
	int in_file = check(open(argv[1], O_RDONLY));
	int out_file = check(open(argv[2], O_WRONLY));
	char* buf = (char*)malloc(BUF_SIZE);
	if (buf == NULL){
		_exit(1);
	}
	int filled = 0;
	while(1){
		int input = check(read(IN, buf + filled, BUF_SIZE - filled));
		cout << "YEAH4!!! " << input << " " << filled << buf << endl;
		if (input == 0 && filled == 0){
			break;
		}
		filled += input;
		int separator = 0;
		for (int i = 0; i < filled; i++) if (buf[i] == '\n'){
			char** command = get_command(buf + separator, i - separator, ' ');
			int fds[2];
			check(pipe(fds));
			execute(command, in_file, fds[1]);
			free(command);
			in_file = fds[0];
			separator = i + 1;
		}
		filled -= separator;
		memmove(buf, buf + separator, filled);
	}
	// need for working
	char** command = (char**)malloc(sizeof(char*) * 2);
	command[0] = (char*)"cat";
	command[1] = NULL;
	execute(command, in_file, 1);
	free(command);
	cout << "YEAH" << endl;
	return 0;
}