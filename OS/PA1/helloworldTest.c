#include <unistd.h>
#include <sys/syscall.h>

int main()
{

if (!syscall(332)) {
 
	return 0;

 }else {

	return -1;

 }

}
