#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#define _XOPEN_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdbool.h>
#include "myassert.h"
#include <semaphore.h>
#include "master_client.h"

#include "comm.h"



// fonctions éventuelles internes au fichier


// fonctions éventuelles proposées dans le .h

int createNamedPipe(char const *name, int right){
    int ret = mkfifo(name,right);
    myassert(ret == 0, "Le tube n'a pas ete creé");
    return ret ;
}

int destroyNamedPipe(char const *name){
    int ret = unlink(name);
    myassert(ret == 0, "Le tube n'a pas ete detruit");
    return ret ;
}


int my_semget()
{
    key_t cle = ftok(MON_FICHIER, PROJ_ID);
    myassert(cle != -1, "Cle non cree");
    int semId = semget(cle, 1, IPC_CREAT | IPC_EXCL | 0641);
    myassert(semId != -1, " Sem non cree");
    int ret = semctl(semId, 0, SETVAL, 0);
    myassert(ret != -1,"semctl");


    return semId ;
}

int recup_semget()
{
    int semId;
    key_t key;
    key = ftok(MON_FICHIER, PROJ_ID);
    myassert(key != -1, "Clé ok");
    semId = semget(key, 1, 0);
    myassert(semId != -1,"Semaphore ok");

    return semId;
}

int my_semget2()
{
    key_t cle2 = ftok(MON_FICHIER, PROJ_ID2);
    myassert(cle2 != -1, "Cle non cree");
    int semId = semget(cle2, 2, IPC_CREAT | IPC_EXCL | 0641);
    myassert(semId != -1, " Sem non cree");
    int ret = semctl(semId, 0, SETVAL, 0);
    myassert(ret != -1,"semctl");


    return semId ;
}

int recup_semget2()
{
    int semId;
    key_t key;
    key = ftok(MON_FICHIER, PROJ_ID2);
    myassert(key != -1, "Clé ok");
    semId = semget(key, 1, 0);
    myassert(semId != -1,"Semaphore ok");

    return semId;
}

int my_semget3()
{
    key_t cle3 = ftok(MON_FICHIER, PROJ_ID3);
    myassert(cle3 != -1, "Cle non cree");
    int semId = semget(cle3, 1, IPC_CREAT | IPC_EXCL | 0641);
    myassert(semId != -1, " Sem non cree");
    int ret = semctl(semId, 0, SETVAL, 0);
    myassert(ret != -1,"semctl");


    return semId ;
}

int recup_semget3()
{
    int semId;
    key_t key;
    key = ftok(MON_FICHIER, PROJ_ID3);
    myassert(key != -1, "Clé ok");
    semId = semget(key, 1, 0);
    myassert(semId != -1,"Semaphore ok");

    return semId;
}

void entrerSC(int semId)
{
    int ret;
    struct sembuf operationMoins = {0, -1, 0};
    ret = semop(semId, &operationMoins, 1);
    myassert(ret != -1,"Entrée SC ok");
}

void sortirSC(int semId)
{
    int ret;
    struct sembuf operationPlus = {0, +1, 0};
    ret = semop(semId, &operationPlus, 1);
    myassert(ret != -1,"Sortie SC ok");
}


void my_destroy_sem(int semId)
{
    int ret;
    ret = semctl(semId, -1, IPC_RMID);
    myassert(ret != -1, "Sem non detruit");
}

void remplir_tab(bool *p, int nombre , int taille){

	for(int k = 2 ; k < taille ; k++){

		if( k != nombre && k % nombre == 0 )
			p[k] = false ;
	}
}

void * PrimeLocal(void * arg)
{
    myassert(arg != NULL,"");
    Thread *pdata = (Thread *) arg;

 remplir_tab(pdata->b , pdata->nombre , *pdata->taille);

    return NULL;

}

void afficheTrueTab(bool tab[], int taille){
    printf("Les cases dont la valeur vaut true sont : \n");
    for (int i = 2 ; i <taille; i++){
        if(tab[i])
            printf("%d \n",i);
    }
}

void * codeThread(void * arg)
{
    Thread *data = (Thread *) arg; 
    pthread_mutex_lock(data->mutex);
    for (int i = 2; i < *(data->taille); i++){
        
	if ( i % data->nombre && (i != data->nombre))
	{
		(data->b)[i] = false ;
		
	}
        
    }
    pthread_mutex_unlock(data->mutex);


    return NULL;
}


