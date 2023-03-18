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

#include "master_client.h"
#include "master_worker.h"



/************************************************************************
 * Données persistantes d'un master
 ************************************************************************/



// on peut ici définir une structure stockant tout ce dont le master
// a besoin

/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/

static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s\n", exeName);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


/************************************************************************
 * boucle principale de communication avec le client
 ************************************************************************/
void loop(int semId ,int semId3, int MasterToWorker , int WorkerToMaster )
{  
   int highest = 2 ;
   int howmany = 0 ; 
   int order;
   int entier = 0 ;
   int reponsefinal ; 
   int ret ;
   int fd ;
   int fd2 = 404 ;
    fd = open("mtc", O_RDONLY);
    // - ouverture des tubes (cf. rq client.c)
    myassert(fd != -1,"ici");
    printf("Le tube est Client vers Master ouvert en lecture\n");
    while ( true )
    // boucle infinie :
    {	
	entrerSC(semId3);	
	// - attente d'un ordre du client (via le tube nommé)
    	char reponse ;
    	ret = read(fd, &reponse, sizeof(char));
    	myassert((ret == sizeof(char)),"1 boucle") ;
    	order = atoi(&reponse);
    	
	    if ( order == 1 )// - si ORDER_STOP
	    {
	    	entier = -1 ;
		ret = write(MasterToWorker, &entier, sizeof(int)); 
		myassert(ret == sizeof(int),"client a ecrit entier");
		sortirSC(semId);

    		fd2 = open("ctm", O_WRONLY); 
    		myassert(fd2 != -1,"ecrit master");
		//. envoyer un accusé de réception au client
		reponsefinal = 404 ;
		ret = write(fd2, &reponsefinal, sizeof(int));
    		myassert(ret == sizeof(int)," master a ecrit");
    		close( fd2) ;
		
		break ;
	    }
	    else if ( order == 2 )// - si ORDER_COMPUTE_PRIME
	    {	    	

		ret = read(fd, &entier, sizeof(int)); // lis l'entier chez client
    		myassert((ret == sizeof(int)) || (ret == 0),"1 boucle") ;
    		
		
		ret = write(MasterToWorker, &entier, sizeof(int)); // transmet entier au worker
		myassert(ret == sizeof(int),"client a ecrit entier");
		 
	    	ret = read ( WorkerToMaster, &reponsefinal , sizeof(int));
	   	myassert(ret == sizeof(int)," worker donne reponse"); 
	   	sortirSC(semId);

	   	fd2 = open("ctm", O_WRONLY);
    		myassert(fd2 != -1,"ecrit master");
	    	printf("Le tube est Master vers Client ouvert en lecture\n");
	    	ret = write(fd2, &reponsefinal, sizeof(int));
	    	myassert(ret == sizeof(int)," master a ecrit");
	    	close( fd2) ;
	    	
	    	if ( reponsefinal > highest )
	    	{
	    		highest = reponsefinal ; 
	    	}
	    	howmany += 1 ;
	    }
    	    else if ( order == 3 )// - si ORDER_HOW_MANY_PRIME   
	    {
	   	sortirSC(semId);

	   	fd2 = open("ctm", O_WRONLY);
    		myassert(fd2 != -1,"ecrit master");
	    	ret = write(fd2, &howmany, sizeof(int));
	    	myassert(ret == sizeof(int)," master a ecrit");
	    	close( fd2) ;

	    	
	    }
    	    else if ( order == 4 )// - si ORDER_HIGHEST_PRIME
	    {	
	   	
	   	sortirSC(semId);
	   	fd2 = open("ctm", O_WRONLY);
    		myassert(fd2 != -1,"ecrit master");    	
	    	ret = write(fd2, &highest, sizeof(int));
    		myassert(ret == sizeof(int)," master a ecrit");
    		close( fd2) ; 	

			
	    }
	    else if ( order == 5 )// - si ORDER_COMPUTE_PRIME_LOCAL
	    {
	    	sortirSC(semId);
	   	fd2 = open("ctm", O_WRONLY);
    		close( fd2) ; 	
	    	
	    }     
    } 
    close(fd);
    
 
   
     
    //entrerSC(semId2);

    // - si ORDER_COMPUTE_PRIME
    //       . récupérer le nombre N à tester provenant du client
    //       . construire le pipeline jusqu'au nombre N-1 (si non encore fait) :
    //             il faut connaître le plus nombre (M) déjà enovoyé aux workers
    //             on leur envoie tous les nombres entre M+1 et N-1
    //             note : chaque envoie déclenche une réponse des workers
    //       . envoyer N dans le pipeline
    //       . récupérer la réponse
    //       . la transmettre au client
    // - si ORDER_HOW_MANY_PRIME
    //       . transmettre la réponse au client
    // - si ORDER_HIGHEST_PRIME
    //       . transmettre la réponse au client
    // - fermer les tubes nommés
    // - attendre ordre du client avant de continuer (sémaphore : précédence)
    // - revenir en début de boucle
    //
    // il est important d'ouvrir et fermer les tubes nommés à chaque itération
    // voyez-vous pourquoi ?
}


/************************************************************************
 * Fonction principale
 ************************************************************************/

int main(int argc, char * argv[])
{
    if (argc != 1)
        usage(argv[0], NULL);
    // - création des sémaphores
    int semId = my_semget(); // semaphore init a 1
    myassert(semId != -1, "Creation sem");
    int semId3 = my_semget3();
    myassert(semId3 != -1, "Creation sem"); 
    // - création des tubes nommés
    int clientToMaster = createNamedPipe("ctm" , 0644) ;
    int masterToClient = createNamedPipe("mtc" , 0644) ;
    // - création des tubes anonymes
    int ret ;
    int MasterToWorker[2];
    ret = pipe(MasterToWorker);
    myassert(ret == 0,"pipe MasterToWorker cree");
    int WorkerToMaster[2];
    ret = pipe(WorkerToMaster);
    myassert(ret == 0,"pipe MasterToWorker cree");
    printf("%d %d\n", clientToMaster , masterToClient);	
    // - création du premier worker
    pid_t retFork;
    retFork = fork();
    myassert(retFork != -1, "fork cree");
    if (retFork == 0 )
    {
	close(WorkerToMaster[0]) ;
	close(MasterToWorker[1]);
	fils (	MasterToWorker[0],WorkerToMaster[1],2);	
	close(WorkerToMaster[1]) ;
	close(MasterToWorker[0]);
   } 
   else
   {
	   loop(semId,semId3, MasterToWorker[1], WorkerToMaster[0] ); 
	   close(WorkerToMaster[0]) ;
	   close(MasterToWorker[1]);
   }
    // destruction des tubes nommés, des sémaphores, ...
    clientToMaster = destroyNamedPipe("ctm");
    masterToClient = destroyNamedPipe("mtc");
    my_destroy_sem(semId) ;
    my_destroy_sem(semId3) ;
    return EXIT_SUCCESS;
}

// N'hésitez pas à faire des fonctions annexes ; si les fonctions main
// et loop pouvaient être "courtes", ce serait bien
