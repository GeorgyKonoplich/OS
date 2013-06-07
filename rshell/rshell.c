#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <memory.h>

int main(){
	int status, s;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; 
	status = getaddrinfo(NULL, "8823", &hints, &servinfo);
	if (status != 0){
		fprintf(stderr, "getaddrinfo error: %sn", gai_strerror(status));
    	exit(1);
	}

	for(res = servinfo;res != NULL; res = res->ai_next) {
    	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    	if (s == -1){
    		continue;
    	}
    	if (bind(s, res->ai_addr, res->ai_addrlen) == 0) break;
    	close(s);
    }
    int opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
    	perror("setsockopt");
    	exit(1);
	}
	if (listen(s, 10) == -1){
		perror("listen");
    	exit(1);
	}

	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	addr_size = sizeof(their_addr);
	int new_s = accept(s, (struct sockaddr *)&their_addr, &addr_size);
	if (new_s == - 1){
		perror("accept");
    	exit(1);
	}
	pid_t pd = fork();
	if (pd){
		close(new_s);
	}else{
		close(s);
		dup2(new_s, 0);
		dup2(new_s, 1);
		dup2(new_s, 2);
		close(new_s);
		std::cout << "OKI-DOKI" << std::endl;
	}
	freeaddrinfo(servinfo);
	return 0;
}