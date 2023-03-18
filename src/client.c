#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "myassert.h"
#include <string.h>
#include <math.h>
#include <pthread.h>


#include "master_client.h"


// chaines possibles pour le premier paramètre de la ligne de commande
#define TK_STOP      "stop"
#define TK_COMPUTE   "compute"
#define TK_HOW_MANY  "howmany"
#define TK_HIGHEST   "highest"
#define TK_LOCAL     "local"

/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/

static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <ordre> [<nombre>]\n", exeName);
    fprintf(stderr, "   ordre \"" TK_STOP  "\" : arrêt master\n");
    fprintf(stderr, "   ordre \"" TK_COMPUTE  "\" : calcul de nombre premier\n");
    fprintf(stderr, "                       <nombre> doit être fourni\n");
    fprintf(stderr, "   ordre \"" TK_HOW_MANY "\" : combien de nombres premiers calculés\n");
    fprintf(stderr, "   ordre \"" TK_HIGHEST "\" : quel est le plus grand nombre premier calculé\n");
    fprintf(stderr, "   ordre \"" TK_LOCAL  "\" : calcul de nombres premiers en local\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

static int parseArgs(int argc, char * argv[], int *number)
{
    int order = ORDER_NONE;

    if ((argc != 2) && (argc != 3))
        usage(argv[0], "Nombre d'arguments incorrect");

    if (strcmp(argv[1], TK_STOP) == 0)
        order = ORDER_STOP;
    else if (strcmp(argv[1], TK_COMPUTE) == 0)
        order = ORDER_COMPUTE_PRIME;
    else if (strcmp(argv[1], TK_HOW_MANY) == 0)
        order = ORDER_HOW_MANY_PRIME;
    else if (strcmp(argv[1], TK_HIGHEST) == 0)
        order = ORDER_HIGHEST_PRIME;
    else if (strcmp(argv[1], TK_LOCAL) == 0)
        order = ORDER_COMPUTE_PRIME_LOCAL;

    if (order == ORDER_NONE)
        usage(argv[0], "ordre incorrect");
    if ((order == ORDER_STOP) && (argc != 2))
        usage(argv[0], TK_STOP" : il ne faut pas de second argument");
    if ((order == ORDER_COMPUTE_PRIME) && (argc != 3))
        usage(argv[0], TK_COMPUTE " : il faut le second argument");
    if ((order == ORDER_HOW_MANY_PRIME) && (argc != 2))
        usage(argv[0], TK_HOW_MANY" : il ne faut pas de second argument");
    if ((order == ORDER_HIGHEST_PRIME) && (argc != 2))
        usage(argv[0], TK_HIGHEST " : il ne faut pas de second argument");
    if ((order == ORDER_COMPUTE_PRIME_LOCAL) && (argc != 3))
        usage(argv[0], TK_LOCAL " : il faut le second argument");
    if ((order == ORDER_COMPUTE_PRIME) || (order == ORDER_COMPUTE_PRIME_LOCAL))
    {
        *number = strtol(argv[2], NULL, 10);
        if (*number < 2)
             usage(argv[0], "le nombre doit être >= 2");
    }

    return order;
}




/************************************************************************
 * Fonction principale
 ************************************************************************/

int main(int argc, char * argv[])
{
    int number = 1;
    int order = parseArgs(argc, argv, &number);
    printf("Voici le code de l'ordre :%d\n", order); // pour éviter le warning

    // order peut valoir 5 valeurs (cf. master_client.h) :
    //      - ORDER_COMPUTE_PRIME_LOCAL
    //    alors c'est un code complètement à part multi-thread

    // sinon
    //    - entrer en section critique :
    //           . pour empêcher que 2 clients communiquent simultanément
    //           . le mutex est déjà créé par le master
    //    - ouvrir les tubes nommés (ils sont déjà créés par le master)

    int fd ;
    int ret ;
    char reponse ;
    int entier ;
    int semId = recup_semget(); // init semaphore //  - envoyer l'ordre et les données éventuelles au master
    
    int semId3 = recup_semget3() ; //    - débloquer le master grâce à un second sémaphore (cf. ci-dessous)
    

    

  // Tube ou le client demande 	DEMANDE
    fd = open("mtc", O_WRONLY);
    myassert(fd != -1,"ecrit client");
    if (order == -1 )//      - ORDER_STOP
    {
    	reponse = '1' ;
    	ret = write(fd, &reponse, sizeof(char));
    	myassert(ret == sizeof(char),"client a ecrit");
    	printf ("Ordre envoyer, on entre en SC \n");
    	sortirSC(semId3);
	entrerSC(semId);
    }
    else if (order == 1 )//      - ORDER_COMPUTE_PRIME
    {
	reponse = '2' ;
	ret = write(fd, &reponse, sizeof(char));
	myassert(ret == sizeof(char),"client a ecrit");

	entier = atoi(argv[2]);
	printf ("Entier envoyer %d \n",entier);
	ret = write(fd, &entier, sizeof(int));
	myassert(ret == sizeof(int),"client a ecrit entier");

	printf ("Ordre envoyer, on entre en SC \n");
	sortirSC(semId3);
	entrerSC(semId);
    }
    else if (order == 2 )//      - ORDER_HOW_MANY_PRIME
    {
	reponse = '3' ;
	ret = write(fd, &reponse, sizeof(char));
	myassert(ret == sizeof(char),"client a ecrit");

	printf ("Ordre envoyer, on entre en SC \n");
	sortirSC(semId3);
	entrerSC(semId);
    }
    else if (order == 3 )//      - ORDER_HIGHEST_PRIME
    {
	reponse = '4' ;
	ret = write(fd, &reponse, sizeof(char));
	myassert(ret == sizeof(char),"client a ecrit");

	printf ("Ordre envoyer, on entre en SC \n");
    	sortirSC(semId3);
	entrerSC(semId);

    }
    else if (order == 4 )// si c'est ORDER_COMPUTE_PRIME_LOCAL
    {
	close(fd);
	//int semId2 = my_semget2();
	int number = atoi(argv[2] );
    	bool tab[number];
    	bool *p;
    	p=tab;
    //////Ton Variable ici ////
    	int nbMaxThread = (int) (number);
    	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    	pthread_t tabId[nbMaxThread];
    	Thread datas[nbMaxThread];
	reponse = '5' ;


	/// Fin de tes Variables //// 
	
	/// Ton code ici ///  
	
        for (int i = 0; i< number ;i++){
            tab[i]= true;
        }

        // pré-initialisation des données
        for (int i = 0; i < number; i++)
        {
            datas[i].nombre = i;
            datas[i].mutex = &mutex;
            datas[i].b= p;
            datas[i].taille = &nbMaxThread;
        }


        // lancement des threads
    	for (int i = 0; i < nbMaxThread; i++)
    	{
        // et donc on passe un pointeur sur une struct différente chaque fois
        int ret = pthread_create(&(tabId[i]), NULL, codeThread, &(datas[i]));
        myassert(ret == 0, "lancement ok");
    	}    	
    	for (int i = 0; i < nbMaxThread; i++)
    	{
        int ret = pthread_join(tabId[i], NULL);
        myassert(ret == 0, "fin ok");
   	 }

    	// pour être propre, on détruit le mutex
    	int ret = pthread_mutex_destroy(&mutex);
    	
    	myassert(ret == 0, "mutex dertuit");

    	afficheTrueTab(tab, number);	
		
	/// Fin de ton code    


	
    }
    close(fd);
    //////////////////////////    2eme partie /////////////

    // Tube ou le master repond RECEPTION
    int reponsefinal ;
    fd = open("ctm", O_RDONLY);
    myassert(fd != -1,"ici");
    if ( order == -1 )//      - ORDER_STOP
    {

	   ret = read(fd, &reponsefinal, sizeof(int));
	   myassert((ret == sizeof(int)) ,"1 boucle");
	   printf("Master fermer %d \n",reponsefinal);
    }
    else if ( order == 1 )//      - ORDER_COMPUTE_PRIME
    {
            ret = read(fd, &reponsefinal, sizeof(int));
	    myassert((ret == sizeof(int)) ,"1 boucle");
	    if ( reponsefinal == -1 )
	    {
	    	printf("Le nombre %d n'est pas premier \n",entier);
	    }
	    else
	    {
	    	printf("Le nombre %d est premier \n",reponsefinal);
	    }
    }
    else if ( order == 2 )//      - ORDER_HOWMANY_PRIME
    {
	   ret = read(fd, &reponsefinal, sizeof(int));
	   myassert((ret == sizeof(int)) ,"1 boucle");
	   printf("HOWMANY %d \n",reponsefinal);
    }
    else if ( order == 3 ) //      - ORDER_HIGHEST_PRIME
    {
	   ret = read(fd, &reponsefinal, sizeof(int));
	   myassert((ret == sizeof(int)) ,"1 boucle");
	   printf("HIGHEST %d \n",reponsefinal);
    }
    else if ( order == 4 ) // si c'est ORDER_COMPUTE_PRIME_LOCAL
    {
		//close(fd);
	   //printf("HIGHEST %d \n",reponsefinal);
    }
    close(fd);
    return EXIT_SUCCESS;
}
