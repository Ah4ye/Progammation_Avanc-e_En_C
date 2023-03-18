#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "myassert.h"

#include "master_worker.h"

/************************************************************************
 * Données persistantes d'un worker
 ************************************************************************/

// on peut ici définir une structure stockant tout ce dont le worker
// a besoin : le nombre premier dont il a la charge, ...
//gcc -Wall -Wextra -pedantic -std=c99 -o nom_du_^Cchier_objet  -c nom_du_fichier_source_C

typedef struct worker{

	int entier;
	int tubeEntrant;
	int tubeMaster ;
	int tubeSortant;
	int highest  ; 
}worker ;


/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/

static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <n> <fdIn> <fdToMaster>\n", exeName);
    fprintf(stderr, "   <n> : nombre premier géré par le worker\n");
    fprintf(stderr, "   <fdIn> : canal d'entrée pour tester un nombre\n");
    fprintf(stderr, "   <fdToMaster> : canal de sortie pour indiquer si un nombre est premier ou non\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

static void parseArgs(int argc, char * argv[] ,worker *work)
{
    if (argc != 4)
        usage(argv[0], "Nombre d'arguments incorrect");

    // remplir la structure
	int fd = atoi(argv[1]);
	work->tubeEntrant = fd;
	fd = atoi(argv[2]);
	work->tubeMaster = fd ;
	fd = atoi(argv[3]);
	work->entier = fd ;
}

/************************************************************************
 * Boucle principale de traitement
 ************************************************************************/

void loop(worker work)
{
    
    int WorkerToWorker[2];
    int ret = pipe(WorkerToWorker);
    myassert(ret == 0,"pipe MasterToWorker cree");
    // boucle infinie :
    while (true)
    {
    //    attendre l'arrivée d'un nombre à tester
	int nombre ;
    	ret = read(work.tubeEntrant, &nombre, sizeof(int));
    	myassert((ret == sizeof(int)) ,"worker lis") ;
    	printf ("    WORKER °%d \n" , work.entier );
    	printf (" NOMBRE A TRAITER %d \n" , nombre );
    	
   	if (nombre == work.entier )
   	{
   		printf(" Nombre %d premier \n", nombre) ;
   	    	int reponse =  nombre ;
    		ret = write(work.tubeMaster, &reponse, sizeof(int));
    		myassert((ret == sizeof(int)) ,"worker ecrit") ;
		work.highest = work.entier ;
    		printf ( " Voici la reponse envoyer au master: %d \n" , reponse ) ;
   	}
   	else if ( nombre == -1)
   	{
   		printf ( " Fermeture declencher, Worker %d fermer \n" , work.entier ) ;
   		ret = write(WorkerToWorker[1], &nombre, sizeof(int));
    		myassert((ret == sizeof(int)) ,"worker ecrit") ;
   		break ;
   	}
   	else if (  nombre%work.entier == 0 )
   	{
   		
	   	int reponse =  -1 ;
    		ret = write(work.tubeMaster, &reponse, sizeof(int));
    		myassert((ret == sizeof(int)) ,"worker ecrit") ;
    		printf ( " Le nombre %d est un multiple de %d \n" , reponse, work.entier ) ;
    		printf(" Nombre %d est non premier \n", nombre) ;	
   	}
   	else
   	{
   		
	    	int ProchainWorker =  AlgoNombrePremier(work.entier) ;
		work.tubeSortant = WorkerToWorker[1];
		
		ret = write(WorkerToWorker[1], &nombre, sizeof(int));
    		myassert((ret == sizeof(int)) ,"worker ecrit") ;
    		
	   	pid_t retFork = fork();
	   	if ( retFork == 0 )
	   	{
			printf(" Creation d'un nouveau Worker \n") ;
   			fils(WorkerToWorker[0],work.tubeMaster,ProchainWorker);	   		
	   	}
	   	
   	} 
    //    si ordre d'arrêt
    //       si il y a un worker suivant, transmettre l'ordre et attendre sa fin
    //       sortir de la boucle
    //    sinon c'est un nombre à tester, 4 possibilités :
    //           - le nombre est premier
    //           - le nombre n'est pas premier
    //           - s'il y a un worker suivant lui transmettre le nombre
    //           - s'il n'y a pas de worker suivant, le créer

    }

    
}

/************************************************************************
 * Programme principal
 ************************************************************************/

int main(int argc, char * argv[])
{
    
    worker work ;
    parseArgs(argc, argv , &work );
    
    // Si on est créé c'est qu'on est un nombre premier
    // Envoyer au master un message positif pour dire
    // que le nombre testé est bien premier        
    
    loop(work);
    

    
    
    // libérer les ressources : fermeture des files descriptors par exemple
	close(work.tubeMaster);
	close(work.tubeEntrant ) ;
	
	
    return EXIT_SUCCESS;
}
