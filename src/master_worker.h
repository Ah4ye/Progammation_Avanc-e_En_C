#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

// On peut mettre ici des éléments propres au couple master/worker :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (écriture dans un tube, ...)

void fils(int fdReadMaster, int fdWriteMaster , int entier);

int highest ( int nombre );

int AlgoNombrePremier(int NumeroWorker) ;
#endif
