

#define _XOPEN_SOURCE 500

#include <limits.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/xattr.h>


typedef struct inputData 
{
	//char *password;
	char *dir;
}inputData;

int main(int argc, char *argv[])
{
	struct inputData *Data = malloc(sizeof(struct inputData));
	
	Data->dir = realpath(argv[1],NULL);
	
	printf("first : %s \n",Data->dir);
	
	Data->dir = argv[1];
	
	printf("Second :%s \n",Data->dir);
	

}
