/*************************************************************************
                           remplirPalette.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la tâche remplirPalette
#if ! defined ( remplirpalette_H )
#define remplirpalette_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <stubs/stubs.h>
#include <mailbox/mailbox.h>
#include <modeles/modeles.h>
#include <log/log.h>
#include <pthread.h>
//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct ArgsRemplirPalette{
  Mailbox<Carton>* balPalette;
  Log * gestionnaireLog;
  Mailbox<Palette>* balStockage;
  Mailbox<Event>* eventBox;
  Mailbox<Message>* balMessages;
  pthread_cond_t *cw;
  pthread_mutex_t* mxcw;
  SharedMemoryLots* shMemLots;
  sem_t * debutSyncro;
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void remplirPalette_thread(void * args);


#endif //REMPLIRPALETTE_H
