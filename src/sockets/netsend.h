/*************************************************************************
                           netsend.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la tache netsend
#if ! defined ( NETSEND_H )
#define NETSEND_H

/////////////////////////////////////////////////////////////////  INCLUDE
#include <modeles/modeles.h>
#include <mailbox/mailbox.h>
#include <log/log.h> 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct NetSendInitInfo
{
    Mailbox<Message> *netmb_ptr;
    int *socket_ptr;
    Log * gestionnaireLog;
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void* thread_netsend(void* arg);

#endif
