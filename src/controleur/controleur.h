/*************************************************************************
                           controleur.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la tâche controleur
#if ! defined ( controleur_H )
#define controleur_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 
#include <pthread.h>
#include <map>
//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 

struct InfoThread
{
  pthread_t id;
  pthread_cond_t * cw;
  pthread_mutex_t * mx;
};

struct ArgControleur
{
  Mailbox<Carton> * balImprimante;
  Mailbox<Carton>  * balPalette;
  Mailbox<Palette> *balStockage;
  Mailbox<Piece> * balPiece;
  Mailbox<Event> * balEvenements;
  Mailbox<Message> * balMessages;
  Mailbox<ListeCommandes> * balCommandes;
  Log * gestionnaireLog;
  std::map<Task,InfoThread> threads;
  Mutex * clapet;
  sem_t * pauseSerieMutex;
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int controleur_thread(void * argsUnconverted);

#endif // controleur_H


