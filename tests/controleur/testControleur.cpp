/*************************************************************************
                           testControleur.cpp  -  Tâche mère
                             -------------------
*************************************************************************/

//---------- Réalisation de  test Controleur ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
//------------------------------------------------------ Include personnel
#include "modeles/modeles.h"
#include "mailbox/mailbox.h"
#include "controleur/controleur.h"
#include "piece/piece.h"
#include "log/log.h"
                             
//------------------------------------------------------ Name spaces
using namespace std;


void task_t(void * a)
{
}

int main()
{
  
  // Initialisation des boites aux lettres
  Mailbox<Event>  balEvenements;
  Mailbox<Carton> balImprimante;
  Mailbox<Carton> balPalette;
  Mailbox<Palette> balStockage;
  Mailbox<Message> balMessages;
  Mailbox<Piece> balPiece;
  Mailbox<ListeCommandes> balCommandes; // tâche destock


  // Mutex
  Mutex  sortieStdMutex;
  Mutex  cartonPresent;
  Mutex  clapet;

  Mutex memLotCourant;

  sem_t  debutSyncro;
  sem_init(&debutSyncro, 0, 0);

  sem_t  pauseSerieMutex;
  sem_init(&pauseSerieMutex, 0, 0);
  

  // Creation du gestionnaire de Log
  Log gestionnaireLog(sortieStdMutex);

  // Allocation des mutex et variables conditionnelles
  pthread_t task,controleur;

  pthread_cond_t taskC=PTHREAD_COND_INITIALIZER; // destocker
 
  pthread_mutex_t taskM=PTHREAD_MUTEX_INITIALIZER; // destocker


  // Variables necessaires à la simulation


  pthread_create (&task, NULL, (void *(*)(void *)) task_t, NULL);


  //Création du thread controleur
  ArgControleur  argControleur;
  argControleur.balEvenements= &balEvenements;
  argControleur.gestionnaireLog=&gestionnaireLog;
  argControleur.balImprimante=&balImprimante;
  argControleur.balPalette=&balPalette;
  argControleur.balStockage=&balStockage;
  argControleur.balPiece=&balPiece;
  argControleur.balCommandes=&balCommandes;
  argControleur.clapet=&clapet;
  argControleur.balMessages=&balMessages;
  argControleur.pauseSerieMutex=&pauseSerieMutex;

  
  InfoThread infoThread;
  infoThread.id =task;
  infoThread.cw=&taskC;
  infoThread.mx=&taskM;
  argControleur.threads[REMPLIRCARTON]=infoThread;
  argControleur.threads[REMPLIRPALETTE]=infoThread;
  argControleur.threads[IMPRIMER]=infoThread;
  argControleur.threads[STOCKERPALETTE]=infoThread;
  argControleur.threads[DESTOCKERPALETTE]=infoThread;

  pthread_create (&controleur, NULL, (void *(*)(void *)) controleur_thread, (void *) &argControleur);


// Tests

  balEvenements.Push(Event(ABSCARTON),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(TAUXERR),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(PANNEIMPRIM),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(FILEATTPLEINE),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(ARTURG),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(PAUSE),0);
  balEvenements.Push(Event(ERRCOMMANDE),0);
  balEvenements.Push(Event(REPRISEPAUSE),0);
  balEvenements.Push(Event(ABSPIECE),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  balEvenements.Push(Event(FINLAST),0);
  balEvenements.Push(Event(REPRISEERREUR),0);
  

// Fin de test
  int a;
  cin>>a;

  balEvenements.Push(Event(FINERREUR),0);
  pthread_cancel(task);


  return 0;
  
}
