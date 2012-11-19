/*************************************************************************
                           imprimer.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la tâche controleur
#if ! defined ( imprimer_H )
#define imprimer_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <pthread.h>
#include <iostream>
#include <modeles/modeles.h>
#include <mailbox/mailbox.h>
#include <log/log.h>
//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
using namespace std;

struct ArgImprimer
{
  Mailbox<Event> * eventBox;
  Mailbox<Carton> * balImprimante;
  Log * gestionnaireLog;
  Mailbox<Carton> * balPalette;
  pthread_cond_t * varCond;
  pthread_mutex_t * mutex;
  SharedMemoryLots * Lots; // vérifier si fin du lot pour tuer tache, à faire
  bool * panneImprim;
  Mutex * mutexPanne;
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int imprimer_thread(void * argsUnconverted);

#endif // imprimer_H
