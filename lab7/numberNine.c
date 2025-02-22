#include <stdio.h>
#include <sys/resource.h>

int main() {
	//define the struct rlimit as limit
    struct rlimit limit;
    		//(int resource, struct rlimite *ptr)
    // RLIMIT_NOFILE
              //This specifies a value one greater than the maximum file
              //descriptor number that can be opened by this process.
              //Attempts (open(2), pipe(2), dup(2), etc.)  to exceed this
              //limit yield the error EMFILE. 
             
    if (getrlimit(RLIMIT_NOFILE, &limit) == 0) {
        printf("Hard limit on open files: %ld\n", (long)limit.rlim_max);
    } else {
        perror("getrlimit failed");
        return 1;
    }
    
    return 0;
}
