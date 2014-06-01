#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <memory.h>
#include <vector>
#include <signal.h>
#define IN 0
#define OUT 1
#define BUF_SIZE 4096
using namespace std;

int in_file, npid;
bool flag;
vector<int> v;

int check(int result){
	if (result < 0){
		_exit(1);
	}
	return result;
}

char** get_command(char* buf, int size){
	int kol = 0;
	for (int i = 0; i <= size; i++) if (buf[i] == ' ' || buf[i] == '|' || buf[i] == '\0'){
		buf[i] = '\0';
		kol++;
	}
	char** command = (char**)malloc(sizeof(char*) * (kol + 1));
	kol = 0;
	int pos = 0;
	for (int i = 0; i <= size; i++) if (buf[i] == '\0'){
		char* tmp = (char*)malloc(i - pos + 1);
		memcpy(tmp, buf + pos, i - pos + 1);
		command[kol++] = tmp;
		pos = i + 1;
	}
	command[kol] = NULL;
	return command;
}


void execute(char** argv, int fd_in, int fd_out){
	int pid = check(fork());
	if (pid){
		if (flag){
			npid = pid;
		}
		if (fd_in != IN) check(close(fd_in));
		if (fd_out != OUT) check(close(fd_out));
	}else{
		check(dup2(fd_in, IN));
		check(dup2(fd_out, OUT));
		execvp(argv[0], argv);
		_exit(1);
	}
}

void work(char* buf, int size){
	flag = false;
	if (buf[0]== 'r' || buf[0] == 'b'){
		int pid = check(fork());
		if (pid){
			if (buf[0] == 'r'){
				check(waitpid(pid, NULL, 0));
			}else{
				v.push_back(pid);
			}

			cout << "WAIT" << endl;
		}else{
			check(setpgid(0, 0));

			int pos = 4;
			for (int i = 4; i <= size; i++) if (buf[i] == '|' || buf[i] == '\0'){
				char** command = get_command(buf + pos, i - pos);
				int fds[2];
				check(pipe(fds));
				execute(command, in_file, fds[1]);
				in_file = fds[0];
				free(command);
				pos = i + 1;
			}
			char** com = (char**)malloc(2 * sizeof(char*));
			com[0] = (char*)"cat";
			com[1] = NULL;
			flag = true;
			execute(com, in_file, 1);
			waitpid(npid, NULL, 0);
			cout << "END_CHILDREN!!!" << endl;
			_exit(1);
 		}
	}
	if (buf[0] == 'w'){
		int num;
		for (int i = 0; i <= size; i++) if (buf[i] == ' '){
			num = atoi(buf + i);
			break;
		}
		waitpid(v[v.size() - num - 1], NULL, 0);
		cout << "WAIT NUM" << endl;
	}
	if (buf[0] == 'k'){
		int num;
		for (int i = 0; i <= size; i++) if (buf[i] == ' '){
			num = atoi(buf + i);
			break;
		}
		killpg(v[v.size() - num - 1], SIGTERM);
		cout << "KILL NUM" << endl;
	}
}

int main(int argc, char** argv){
	char* buf = (char*)malloc(BUF_SIZE);
	if (buf == NULL){
		_exit(1);
	}
	int filled = 0;
	while(1){
		int input = check(read(IN, buf + filled, BUF_SIZE - filled));
		cout << "READ!!! " << input << " " << filled << buf << endl;
		if (input == 0 && filled == 0){
			break;
		}
		filled += input;
		int pos = 0;
		in_file = 0;
		for (int i = 0; i < filled; i++) if (buf[i] == '\n'){
			buf[i] = '\0';
			work(buf + pos, i - pos);
			cout << "YEAH3!!!" << endl;
			pos = i + 1;
		}
		filled -= pos;
		memmove(buf, buf + pos, filled);
	}
	free(buf);
}