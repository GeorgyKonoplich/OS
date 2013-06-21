#include <unistd.h>
#include <chrono>
#include <algorithm>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
using namespace std;
#define MAX_SIZE 4096

struct RateLimit{
	int n;
	char* x;
	vector<pair<char*, int> > v;
	RateLimit(size_t n):n(n){
		x = (char*)malloc(MAX_SIZE);
		if (x == NULL){
			exit(0);
		}
	}
	void run(){
		auto stop1 = chrono::high_resolution_clock::now();
		auto start1 = stop1;
		int occupied = 0;
		bool ff = true;
		while (true){
			int input = read(0, x + occupied, MAX_SIZE - occupied);
			if (input < 0){
				exit(0);
			}
			if (input == 0 && occupied == 0){
				break;
			}
			occupied += input;
			int pos_del = -1;
			int i = 0;
			while (occupied > 0 && i < occupied){
				while(i < occupied){
					if (x[i] == '\n'){
						char *st = (char*)malloc(i + 1);
						memcpy(st, x, i + 1);
						v.push_back(make_pair(st, i + 1));
						occupied = occupied - i - 1;
						memmove(x, x + i + 1, occupied);
						i = -1;
					}
					if (v.size() == n) break;
					i++;
				}
				if (v.size() == n){
						while (true){
							stop1 = chrono::high_resolution_clock::now();
							double timer = chrono::duration<double>(stop1 - start1).count();		
							if (timer > 1 || ff) break;
						}
						ff = false;
						for (int j = 0; j < n; j++)
							write(1, v[j].first, v[j].second);
						v.clear();
						start1 = chrono::high_resolution_clock::now();
				}else if (v.size() < n){
					while (true){
							stop1 = chrono::high_resolution_clock::now();
							double timer = chrono::duration<double>(stop1 - start1).count();		
							if (timer > 1 || ff) break;
					}
					for (int j = 0; j < v.size(); j++)
						write(1, v[j].first, v[j].second);
					v.clear();
				}
			}
		}
	}
	~RateLimit(){
		if (x)
			free(x);
	}
};

int main(int argc, char* argv[]){
	RateLimit k(atoi(argv[1]));
	k.run();
	return 0;
}