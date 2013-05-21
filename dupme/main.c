#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

void double_write(char * buffer, int start, int end){
    int i;
    int x = start;
    for (i = 0; i < 2; i++){
        while (start < end){
            int write_res = write(1, buffer + start, end - start);
            if (write_res == -1){
                _exit(3);
            }
            start += write_res;
        }
        start = x;
    }
}

int main(int argc, char** argv){
	if (argc < 2){
		return 1;
	}
	int k, error;
	int len = 0;
	k = atoi(argv[1]);
	if (k < 0) k = 0; else k++;
	char* buf = (char*) malloc(k);
	while(1){
		int input = read(0, buf + len, k - len);
		if (input >= 0) len =  len + input;
		if (input == 0 && len == 0){
			break;
		} else if (input == 0 && error == 0){
			buf[len] = '\n';
			len = len + 1;
		}
		int m = -1;
		int i;
		for (i = 0; i < len; i++) if (buf[i] == '\n'){
			if (error == 1){
				error = 0;
				m = i; 
				break;
			}
			double_write(buf, m + 1, i + 1);
			m = i;
		}
		if (m == -1 && len == k){
			len = 0;
			error = 1;
		}else{ 
			len = len - m - 1;
			memmove(buf, buf + m + 1, len);
		}
	}
	free(buf);
	return 0;
}	

