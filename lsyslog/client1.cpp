#include <stdio.h>
#include "lsyslog.h"

int main() {
	int k = 100000000;
	while (k > 0){
	    openlog("dosvidos", 2, 2);
    	syslog(26, "dosvidos");
    	k--;
    }
   
}
