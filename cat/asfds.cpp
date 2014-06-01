#include <errno.h>
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

char* my_itoa(int x){
	int g[10];
	int kol = 0;
	while (x > 0){
		g[kol++] = x % 10;
		x = x / 10;
	}
	char* ans = (char*)malloc(kol + 1);
	if (ans == NULL){
		_exit(EXIT_FAILURE);
	}
	for (int i = kol - 1; i >= 0; i--){
		ans[kol - i - 1] = g[i] + '0';
	}
	ans[kol] = '\0';
	return ans;
}

int main(int argc, char* argv[]){
	if (argc < 2){
		_exit(EXIT_FAILURE);
	}
	char **args = (char**)malloc((argc) * sizeof(char*));
	if (args == NULL){
		_exit(EXIT_FAILURE);	
	}
	int k = argc - 2;
	for (int i = 0; i < k; i++){
		int file = open(argv[i + 2], O_RDONLY);
		if (file == -1){
			_exit(EXIT_FAILURE);
		}
		char *f1 = my_itoa(file);
		args[i + 1] = f1;
	}
	args[argc - 1] = NULL;
	args[0] = argv[1];
	int pid = fork();
	if (pid == -1){
		_exit(EXIT_FAILURE);
	}else
	if (pid == 0){
		execvp(args[0], args);
		_exit(errno);
	}else{
		int status;
		int id = wait(&status);
		if (id == -1){
			_exit(EXIT_FAILURE);
		}
	}
	for (int i = 1; i < argc - 1; i++){
		int fd = atoi(args[i]);
		close(fd);
	}
	free(args);
	return 0;
}