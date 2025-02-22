#include <stdio.h>
#include <unistd.h>

int main(){
	//use sysconf() with the POSIX variable _SC_PAGESIZE
	//PAGESIZE - _SC_PAGESIZE
              //Size of a page in bytes.  Must not be less than 1.
	printf("The page size bytes are: %ld \n", sysconf(_SC_PAGESIZE));
	return 0;
}