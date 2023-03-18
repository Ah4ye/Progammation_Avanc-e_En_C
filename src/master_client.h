#ifndef CLIENT_CRIBLE
#define CLIENT_CRIBLE

// On peut mettre ici des éléments propres au couple master/client :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (création tubes, écriture dans un tube,
//      manipulation de sémaphores, ...)
#include <pthread.h>
// ordres possibles pour le master
#define ORDER_NONE                0
#define ORDER_STOP               -1
#define ORDER_COMPUTE_PRIME       1
#define ORDER_HOW_MANY_PRIME      2
#define ORDER_HIGHEST_PRIME       3
#define ORDER_COMPUTE_PRIME_LOCAL 4   // ne concerne pas le master

// bref n'hésitez à mettre nombre de fonctions avec des noms explicites
// pour masquer l'implémentation
int createNamedPipe(char const *name, int right);

int destroyNamedPipe(char const *name);

int my_semget();
int my_semget2();
int my_semget3();

int recup_semget();
int recup_semget2();
int recup_semget3();

void entrerSC(int semId);

void sortirSC(int semId);

void my_destroy_sem(int semId);

typedef struct Thread
{
    int nombre;
    bool *b;
    int *taille;
    pthread_mutex_t *mutex;
}Thread;

void * codeThread(void * arg);

void afficheTrueTab(bool tab[], int taille);
#endif
