#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 16
int main()
{
size_t usernameSize = 0;
char *data1;
printf ("Please input username:\n");
getline(&data1, &usernameSize, stdin);
// remove the new line character
data1[strlen(data1) - 1] = '\0';
printf ("You entered: [%s]\nsize of username: %ld\n", data1, strlen(data1));
free (data1);
return 0;
}
