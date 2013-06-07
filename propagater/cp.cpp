#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <memory.h>

char* my_itoa(int x){
	int g[10];
	int kol = 0;
	while (x > 0){
		g[kol++] = x % 10;
		x = x / 10;
	}
	char* ans = (char*)malloc(kol + 1);
	for (int i = kol - 1; i >= 0; i--){
		ans[kol - i - 1] = g[i] + '0';
	}
	ans[kol] = '\0';
	return ans;
}
int main(int argc, char **argv){
	char **args = (char**)malloc((argc + 1) * sizeof(char*));
	for (int i = 1; i < argc; i+=2){
		int file1 = open(argv[i], O_RDONLY);
		int file2 = open(argv[i+1], O_WRONLY);
		char *f1;
		char *f2;
		f1 = my_itoa(file1);
		f2 = my_itoa(file2);
		args[i] = f1;
		args[i + 1] = f2;
	}
	args[argc] = NULL;
	char *st = "./propagater\0";
	args[0] = st;
	int pid = fork();
	if (pid == 0){
		execvp(args[0], args);
		exit(0);
	}
	return 0;
}