#include <unistd.h>
#include <sys/types.h>
//#include <sys/syscall.h>
#include <stdio.h>
//#include <linux/kernel.h>
#include <stdlib.h>
#include <fcntl.h>


#define BUFFER_SIZE 1024



int main(){
	char *userbuffer;
	char Buffer[BUFFER_SIZE];
	int file1 = open("/dev/simple_character_device", O_RDWR);
	int fd = 0;
	int offset;
	int whence;

	char command;
	while (command != 'e') {
		printf("__________________________________\n\n");
		printf(" Press r to read from device\n");
		printf(" Press w to write to the device\n");
		printf(" Press s to seek into the device\n");
		printf(" Press e to exit from device\n");
		printf(" Enter Command: ");	
		scanf(" %s",&command);
		printf("__________________________________\n");

		
		

		if (command == 'e'){
			printf("\nGoodbye! \n");
		}else if (command == 'r'){
			
			printf("Enter the Number of Bytes you want to read:\n");
			int bytesToRead;
			scanf("%d",&bytesToRead);
			//userbuffer = malloc(bytesToRead);

			int a = read(file1,Buffer,bytesToRead);

			
		

			//printf("\n Number of bytes read = %d.\n",bytesToRead);
			//printf("%d",fd);
			printf("\n read %d  bytes \n",bytesToRead);
			printf("\n %s \n",Buffer);
			//printf("\n %s \n",userbuffer);
			
			while(getchar() != '\n');

			//break;
	

			//code for the read function here, read too userbuffer

		}else if (command == 'w'){
			printf("Enter data you want to write to the device: ");
			
			//char input[];

			scanf("%s",Buffer);
			write(file1, Buffer, BUFFER_SIZE);
			
			while(getchar() != '\n'); 
			//break;
			
			 

		}else if (command == 's'){
			

			printf("Enter an offset value: \n");
			scanf("%d",&offset);
			printf("Enter an value for whence: \n");
			scanf("%d",&whence);
			int a = llseek(file1,offset,whence);
			//code for the seek function, pass in offset and whence 

		}else {
			printf(" %c is not a valid command!\n Please enter a valid command.\n", command);
			
		}


	}
	close(file1);
	return 0;
}

