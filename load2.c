#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <malloc.h>
#include "mystruct.h"



int main()
{
	void *shared_memory=(void *)0;
	struct shared_use_st *shared_stuff;
	int shmid;

	FILE *f;
	long long a,b;
	
	/*创建共享内存*/
	shmid=shmget((key_t)5678,sizeof(struct shared_use_st),0666|IPC_CREAT);
	if(shmid==-1)
	{
	    fprintf(stderr,"shmget failed\n");
		exit(EXIT_FAILURE);
	}

	/*映射共享内存*/
	shared_memory=shmat(shmid,(void *)0,0);
	if(shared_memory==(void *)-1)
	{
	    fprintf(stderr,"shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at %X\n",(int)shared_memory);

	/*让结构体指针指向这块共享内存*/
	shared_stuff=(struct shared_use_st *)shared_memory;
	
	/*读取二进制文件 */
	f = fopen("GoogleNews-vectors-negative300.bin","rb");	
	if (f == NULL) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf(f, "%lld", &shared_stuff->words);
	fscanf(f, "%lld", &shared_stuff->size);
	shared_stuff->vocab = (char *)malloc((long long)shared_stuff->words * max_w * sizeof(char));
	

    shared_stuff->M = (float *)malloc((long long)shared_stuff->words * (long long)shared_stuff->size * sizeof(float));
    if(shared_stuff->M == NULL)
	{
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)shared_stuff->words * shared_stuff->size * sizeof(float) / 1048576, shared_stuff->words, shared_stuff->size);
		return -1;
	}
	for (b = 0; b < shared_stuff->words; b++)
	{
		fscanf(f, "%s%c", &shared_stuff->vocab[b * max_w], &shared_stuff->ch);
		//fread是从一个文件流中读数据，数据存在M[a + b * size]中
		for (a = 0; a < shared_stuff->size; a++) fread(&shared_stuff->M[a + b * shared_stuff->size], sizeof(float), 1, f);
		shared_stuff->len = 0;
		for (a = 0; a < shared_stuff->size; a++) shared_stuff->len += shared_stuff->M[a + b * shared_stuff->size] * shared_stuff->M[a + b * shared_stuff->size];
		shared_stuff->len = sqrt(shared_stuff->len);
		for (a = 0; a < shared_stuff->size; a++) shared_stuff->M[a + b * shared_stuff->size] /= shared_stuff->len;
	}

	fclose(f);

	printf("load success!");
		
	while(shared_stuff->words)
	{	
		sleep(1);
	}	
	
	/*删除共享内存*/
	if(shmdt(shared_memory)==-1)
	{
	    fprintf(stderr,"shmdt failed\n");
	    exit(EXIT_FAILURE);
	}
    exit(EXIT_SUCCESS);

}
