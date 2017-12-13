#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <stdlib.h>

int main()
{
int a = 2;
int b = 2;
int result;

printf("%ld\n",syscall(333,a,b,&result));

return 0;


}
