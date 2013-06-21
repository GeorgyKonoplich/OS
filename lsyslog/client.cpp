#include <stdio.h>
#include "lsyslog.h"

int main() {
	int k = 1000000;
	while (k > 0){
	    openlog("hello", k, 1);
    	syslog(13, "hello");
    	k--;
    }
   
}
