
/*************************************************************************
                   network  -  Tâche de reception réseau
*************************************************************************/

//---------- Interface de la tâche network -------
#if ! defined ( network_H )
#define network_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <modeles/modeles.h>
#include <mailbox/mailbox.h>
#include <log/log.h> 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct NetworkInitInfo 
{
    Mailbox<Event> * balEvenements;
    Mailbox<Message> *netmb_ptr;
    Mailbox<ListeCommandes> *balCommandes;
    int *socket_ptr;
    SharedMemoryLots *shMemLots;
    sem_t * debutSyncro;
    Log * gestionnaireLog;
};


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// thread tâche réception
void* thread_network(void* arg);

#endif
