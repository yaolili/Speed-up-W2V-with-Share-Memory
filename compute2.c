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

int main(int argc, char **argv)
{
    
    void *shared_memory=(void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;

    long long a,b,c,d,cn,bi[100];
    float bestd[N],vec[max_size],dist;
    char st1[max_size],st[100][max_size],*bestw[N];

    
    /*创建共享内存*/
    shmid=shmget((key_t)1234567890,sizeof(struct shared_use_st),0666|IPC_CREAT);
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
    //printf("Memory attached at %X\n",(int)shared_memory);

    /*让结构体指针指向这块共享内存*/
    shared_stuff=(struct shared_use_st *)shared_memory;
    
    //printf("%lld %lld",shared_stuff->words,shared_stuff->size);
    
    if (argc < 2)
    {
        printf("Usage: ./compute <QUERY>\nwhere QUERY is what the user search for\n");
        return 0;
    }
    for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));

    /*获取最相近的词*/
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    strcpy(st1, argv[1]);
    if (!strcmp(st1, "EXIT")) return -1;
    cn = 0;
    b = 0;
    c = 0;
    while (1)
    {
        st[cn][b] = st1[c];
        b++;
        c++;
        st[cn][b] = 0;
        if(st1[c] == 0) break;
        if(st1[c] == ' ')
        {
            cn++;
            b = 0;
            c++;
        }
    }
    cn++;
    for (a = 0; a < cn; a++)
    {
        for (b = 0; b < shared_stuff->words; b++) if (!strcmp(&shared_stuff->vocab[b * max_w], st[a])) break;
        if (b == shared_stuff->words) b = -1;
        bi[a] = b;
        //printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
        if (b == -1)
        {
            printf("Out of dictionary word!\n");
            break;
        }
    }
    if (b == -1) return -1;
    for (a = 0; a < shared_stuff->size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++)
    {
        if (bi[b] == -1) continue;
        for (a = 0; a < shared_stuff->size; a++) vec[a] += shared_stuff->M[a + bi[b] * shared_stuff->size];
    }
    shared_stuff->len = 0;
    for (a = 0; a < shared_stuff->size; a++) shared_stuff->len += vec[a] * vec[a];
    shared_stuff->len = sqrt(shared_stuff->len);
    for (a = 0; a < shared_stuff->size; a++) vec[a] /= shared_stuff->len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < shared_stuff->words; c++) 
    {
        a = 0;
        for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
        if (a == 1) continue;
        dist = 0;
        for (a = 0; a < shared_stuff->size; a++) dist += vec[a] * shared_stuff->M[a + c * shared_stuff->size];
        for (a = 0; a < N; a++)
        {
            if (dist > bestd[a]) 
            {
                for (d = N - 1; d > a; d--) 
                {
                    bestd[d] = bestd[d - 1];
                    strcpy(bestw[d], bestw[d - 1]);
                }
                bestd[a] = dist;
                strcpy(bestw[a], &shared_stuff->vocab[c * max_w]);
                break;
            }
        }
    }
    //for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);   
    for (a = 0; a< N; a++) printf("%s\n",bestw[a]);
    
    /*删除共享内存*/
    if(shmdt(shared_memory)==-1)
    {
        fprintf(stderr,"shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
