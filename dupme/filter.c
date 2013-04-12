#include<stdlib.h>
#include<unistd.h>

int main(int argc, char** argv){
    int k, input, output, m, error, i;
    int len = 0;
    if (argv[2] != '-b') k = 4000; else k = atoi(argv[3]);
    if (argv[1] == '-n') temp = '\n;
	int k, input, output, m, error, i;
	int len = 0;
	if (argc == 1) k = 0; else k = atoi(argv[1]);
	if (k < 0) k = 0;
	char* buf = (char*) malloc(k + 2);
	while(1){
		input = read(0, buf + len, k - len + 1);
		for (i = 0; i < len; i++) if (buf[i] == '\n'){
            
        	if (error == 1){
				error = 0;
				m = i; 
				break;
			}
			output = write(1, buf + m + 1, i - m);
			output = write(1, buf + m + 1, i - m);
			m = i;
		}
		if (m == -1 && len == k + 1){
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

