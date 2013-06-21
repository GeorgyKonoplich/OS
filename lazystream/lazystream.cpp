#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <boost/optional.hpp>
using namespace std;


struct my_vector{
	char *x;
	size_t max_size, occupied;

	my_vector(size_t s):max_size(s), occupied(0) {
		x = (char*)malloc(max_size);
		if (x == NULL){
			exit(0);
		}
	}
	
	my_vector(my_vector &&v):max_size(v.max_size), occupied(v.occupied), x(v.x) {
		v.x = NULL;
	}

	~my_vector() {
		if (x)
			free(x);
	}
};

struct Lazy_Sream{
	size_t buf_size;
	int fd;
	char del;
	int len;
	int start = 0;
	my_vector buffer;
	
	Lazy_Sream(size_t buf_size, char del, int fd):
		buf_size(buf_size), del(del), fd(fd), len(0), buffer(buf_size){}
	
	my_vector readnext(){
		while (true){
			int input;
			if (start == buffer.occupied) input = read(fd, buffer.x + start, buf_size - start);
			if (input < 0){
				exit(0);
			}
			buffer.occupied += input; 
			int position_delimetr = - 1;
			for (int i = start; i < buffer.occupied; i++) if (buffer.x[i] == del){
				position_delimetr = i;
				break;
			}
			if (position_delimetr != -1){
				position_delimetr++;
				my_vector newb(buffer.occupied - position_delimetr);
				memcpy(newb.x, buffer.x + position_delimetr, buffer.occupied - position_delimetr);
				buffer.occupied = position_delimetr - 1;
				my_vector cc = move(buffer);
				buffer.x  = move(newb.x);
				buffer.occupied = newb.max_size;
				start = position_delimetr;
				return cc;
			} else start = buffer.occupied;	
			//if (buffer.occupied == buf_size) return cc;//nothing
		}
		//return move(buffer);
	}
};

int main (){
	int file = open("input.txt", O_RDONLY);
	Lazy_Sream t(4096, ' ', file);
	cout << t.readnext().occupied << endl;
	return 0;
}
