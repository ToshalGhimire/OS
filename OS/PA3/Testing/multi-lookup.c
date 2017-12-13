#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>

#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

#include "util.h"
#define BUFSIZE 15

void *Requestor(void *nameFiles);
void *Resolver(void *result);
int getlines(FILE *fileptr);

pthread_mutex_t reqLOCK;
pthread_mutex_t resLOCK;
pthread_mutex_t writing;
pthread_mutex_t lockE;
pthread_mutex_t lock;
pthread_cond_t wr;
pthread_cond_t empty;
//sem_t s;

struct host_ip{
	char hostname[BUFSIZE];
	char *namefile;
	FILE *Result;
	FILE *Service;
	char hasStarted;//used to make sure r
	
	
};



int main(int argc, char **argv){
	printf("*** Code Started *** \n");
	//char hostname[BUFSIZE];
	//char ipstring[BUFSIZE];
	
	
    	struct host_ip *Sharredarray;
    	Sharredarray = (struct host_ip *)malloc(sizeof(struct host_ip));
	
	//Sharredarray->hasStarted = 0;
	Sharredarray->Service = fopen(argv[4],"w");
	Sharredarray->Result = fopen(argv[3],"w");
	
	int RequesterNUM = atoi(argv[1]);
	int ResolverNUM = atoi(argv[2]);
	
	int inputFileCount = argc - 5;
	char* nameXInput[inputFileCount];
	
	
	
	int i,j;
	for(i=0; i<inputFileCount; i++){
		nameXInput[i] = argv[i+5];

	}
	
	pthread_mutex_init(&reqLOCK, NULL); //used to make sure resolver doesnt start while requester is runing
	pthread_mutex_init(&resLOCK, NULL); //used to make sure requester doesnt start while resolver is running
	
	pthread_mutex_init(&lock, NULL);//not sure if nessarry, used in line 90 and 92 
					//UPDATE: it is or sharred array doesnt work properaly
	
	//cond variable wr plus its mutex
	pthread_mutex_init(&writing,NULL); //used to stop requester from inserting new hosts into the shareed array until resolver has put it in restult.txt
	pthread_cond_init(&wr, NULL);//used inside the forloops for resolver/requestor, so the loop doesnt contunie unless the buffer is delt with
	
	//cond variable empty plus its mutex
	pthread_mutex_init(&lockE,NULL);//if sharedarray is empty, then it waits in resolver
	pthread_cond_init(&empty, NULL);// used to deal with when their is empty buffers 
	

	//sem_init(&s, 0, 5);
	

/*--------------------------Threads init-------------------------------------*/	
	pthread_t requestor[RequesterNUM];
	pthread_t resolver[ResolverNUM];
	
/*--------------------------creating Threads-------------------------------------*/
	
	//Request Threads
	printf("after pthreads init \n");
	for (i=0; i < RequesterNUM; i++)
		{
			pthread_mutex_lock(&lock);
			Sharredarray->namefile = nameXInput[i];
			pthread_mutex_unlock(&lock);
			printf("NameXinput is %s \n",nameXInput[i]);
			printf("^^^^^^ Sharredarray->namefile is %s ^^^^^^\n",Sharredarray->namefile);
			
			pthread_create(&requestor[i], NULL , &Requestor, Sharredarray);
			
		};
		
		
	//Resolver Threads
	for (j=0; j < ResolverNUM ; j++)
	{	
		pthread_create(&resolver[j], NULL , &Resolver, Sharredarray);
	};
	
	printf("after call to threads \n");
/*--------------------------Joining Threads-------------------------------------*/
	//Request Threads
	for (i=0; i < RequesterNUM; i++)
	{
		pthread_join(requestor[i],NULL);  
	};
	
	//Resolver Threads
	for (j=0; j < ResolverNUM ; j++)
	{	
		pthread_join(resolver[j],NULL);  
	};
	
	printf("***Code Is Finished*** \n");
	
	fclose(Sharredarray->Service);
	fclose(Sharredarray->Result);
	//fclose(argv[3]);
	//fclose(argv[4]);
	
	pthread_mutex_destroy(&reqLOCK);
	pthread_mutex_destroy(&resLOCK);
	
	pthread_mutex_destroy(&lock);
	
	pthread_cond_destroy(&wr);
	pthread_mutex_destroy(&writing);
	
	pthread_mutex_destroy(&lockE);
	pthread_cond_destroy(&empty);
	
	free(Sharredarray);
}

//thread functions 

void *Requestor(void *data){
	
	
	pthread_mutex_lock(&resLOCK);
	printf("\n-------Requestor thread %ld has started------- \n \n",gettid());
	
	int fileserviced = 0;
	char *inputfilename;
	
	struct host_ip *DATA = data;
	//pthread_mutex_lock(&lock);
	
	FILE *a = fopen(DATA->namefile,"r");
	fprintf(DATA->Service,"Thread %ld  serviced %s file.\n", gettid(),DATA->namefile);
	
	
	
	DATA->hasStarted = 1;
	inputfilename = DATA->namefile;
	
	printf("&&&&&&& Sharredarray->namefile is name%s ;&&&&&&&&&&&& %s &&&&&&&&&&&&&&& \n",DATA->namefile,inputfilename);
	
	

	int lines = getlines(a);
	
	
	for(int j = 0; j < lines  ;j++){
		
		fscanf(a, "%s",DATA->hostname);
		printf("Host: %s has been put into array\n",DATA->hostname);
		pthread_cond_signal(&empty);//signals resolver, to notify that buffer inst full anymore
		pthread_cond_wait(&wr, &writing);//waits for resolver to finish servcing the host name
	
	}
	
	printf("\n-------Requestor thread %ld has Finished------- \n \n",gettid());
	DATA->hasStarted = 0;
	//pthread_mutex_unlock(&lock);
	pthread_mutex_unlock(&resLOCK);
	
}


void *Resolver(void *data){
	
	pthread_mutex_lock(&reqLOCK);
	struct host_ip *DATA = data;
	while(!DATA->hasStarted);
	printf("\n-------Resolver thread %ld has started -------\n \n",gettid());
	
	char ipstring[BUFSIZE];
	FILE *Inputfile = fopen(DATA->namefile,"r");
	
	int lines = getlines(Inputfile);
	
	
	for(int j = 0; j < lines ;j++){
		
		while(DATA->hostname[0] == '\0') { //if Sharred array is empty it locks resolver
			pthread_cond_wait(&empty,&lockE);
			}
		
		dnslookup(DATA->hostname, ipstring, 20);
		fprintf(DATA->Result, "%s,%s \n",DATA->hostname,ipstring);	
		printf("%s has been posted to Result.txt\n \n",DATA->hostname);
		DATA->hostname[0] = '\0';//removes host name from array then signals requestor to put another 
		pthread_cond_signal(&wr);
	
	}
	//pthread_cond_signal(&wr);
	printf("\n-------Resolver thread %ld has Finished------- \n \n",gettid());
	pthread_mutex_unlock(&reqLOCK);
}

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
