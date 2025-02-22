#include <stdio.h>
#include <time.h>

int main() {
	//create a struct timespec called res
    struct timespec res;
    
    	//the clock_getres() takes two arguements
    	// 1. CLOCK_REALTIME: A settable system-wide clock that measures real (i.e.,
              //wall-clock) time.  Setting this clock requires appropriate privileges.
        //2. the pointer to timespec res struct
    if (clock_getres(CLOCK_REALTIME, &res) == 0) {
        printf("Clock resolution: %ld ms\n", res.tv_sec / 1000);
    } else {
        perror("clock_getres failed");
        return 1;
    }
    
    return 0;
}
