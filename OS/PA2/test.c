#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

#define BUFFER_SIZE 1024

int main() {
	char option, buffer[BUFFER_SIZE];
	int whence, offset, userbuffer;
	int file = open("/dev/simple_character_device", O_RDWR);

	printf("Welcome to test\n");

	while(1){
		printf("\nCommand: ");
		scanf("%c", &option);
		switch (option){
			case 'r':
				//printf("Enter the number of bytes you want to read: \n");
				//scanf("%d", buffer);
				read(file, buffer, BUFFER_SIZE);
				printf("Data read from device: %s\n", buffer);
				while (getchar() != '\n');
				break;
			case 'w':
				printf("Enter data to write to device: ");
				scanf("%s", buffer);
				write(file, buffer, BUFFER_SIZE);
				while (getchar() != '\n');
				break;
			case 's':
				printf("Enter offset value: \n");
				scanf("%d", &offset);
				printf("Enter a value for whence: \n");
				scanf("%d", &whence);
				llseek(file, offset, whence);
				while (getchar() != '\n');
				break;
			case 'e':
				printf("Exiting test\n");
				return 0;
			default:
				while (getchar() != '\n');
		}
	}
	close(file);
	return 0;
}
