#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSIZE 15
#include <string.h>

#include "util.h"
int getlines(FILE *fileptr);

int getlines(FILE *fileptr){
	int lines = 0;
	int ch;
	while(!feof(fileptr))
	{
		ch = fgetc(fileptr);
		if(ch == '\n')
		{
			lines++;
		}
	} rewind(fileptr);
	return lines;
}

int main(int argc,int **argv) {

	char hostname[BUFSIZE];
	char ipstring[BUFSIZE];
	
	FILE *input = fopen("names1.txt","r");
	
	int lines = getlines(input);
	
	printf("%d \n",lines);

	
	FILE *results = fopen("Results.txt","w");
	
	int ip = 1;
	for(int j = 0; j < lines ;j++){
		
		fscanf(input,"%s",hostname);
		dnslookup(hostname, ipstring, 20);
		fprintf(results, "%s,%s \n",hostname,ipstring);	
	}rewind(input);
	
	


	fclose(input);
	fclose(results);
}


