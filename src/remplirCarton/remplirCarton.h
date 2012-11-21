
/*************************************************************************
                           remplirCarton  -  Tâche qui gère le remplir
*************************************************************************/

//---------- Interface de la tâche remplir carton -------
#if ! defined ( remplirCarton_H )
#define remplirCarton_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
typedef bool (*stubFunction)(void);

struct ArgRemplirCarton
{
    Mailbox<Piece>* pBalPieces;
    Mailbox<Carton>* pBalCartons;
    Mailbox<Event>* pBalEvenements;
    Mailbox<Message>* pBalMessages;
    Log * gestionnaireLog;
    stubFunction capteurCarton;
    Mutex * lotCourantMutex;
    SharedMemoryLots *shMemLots;
    int * lotCourant;
    pthread_cond_t* cv;
    pthread_mutex_t* mutCv;
    sem_t * debutSyncro;
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void * remplirCarton(void * index);


#endif // remplirCarton_H


