#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "myassert.h"

#include "master_worker.h"

// fonctions éventuelles internes au fichier



void fils(int fdReadMaster, int fdWriteMaster , int entier)
{
    char tubeEntrant[1000];
    char tubeSortant[1000];
    char valeur[1000];
    char *argv[5];

    sprintf(tubeEntrant, "%d", fdReadMaster);
    sprintf(tubeSortant, "%d", fdWriteMaster);
    sprintf(valeur,"%d", entier);

    argv[0] = "worker";
    argv[1] = tubeEntrant;
    argv[2] = tubeSortant;
    argv[3] = valeur;
    argv[4] = NULL;

    execv(argv[0], argv);
    perror("pb exec fils");
}



int AlgoNombrePremier(int NumeroWorker)
{
	int resultat ;
	NumeroWorker += 1 ;
	bool roule = true ;
	int k ;
	while ( roule )
	{
		for ( k = 2 ; k < NumeroWorker ; k++ ) 
		{
			if ( NumeroWorker % k == 0)
			{
				break;
			}
			if ( k == NumeroWorker -1)
			{
				roule = false	;
			}
		}
		if ( roule == true)
		{
			 NumeroWorker++ ;
		}
	} 
	resultat = NumeroWorker ;
	return	resultat ;	
}






// fonctions éventuelles proposées dans le .h
