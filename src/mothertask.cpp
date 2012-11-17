/*************************************************************************
                           mothertask.cpp  -  Tâche mère
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche mère ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
//------------------------------------------------------ Include personnel
#include "modeles/modeles.h"
#include "mailbox/mailbox.h"
#include "controleur/controleur.h"
#include "sockets/network.h"
#include "sockets/netsend.h"
#include "imprimer/imprimer.h"
#include "log/log.h"
#include "remplirCarton/remplirCarton.h"
#include "remplirPalette/remplirPalette.h"
#include "gestionserie/gestionserie.h"
#include "stock/stock.h"
#include "destock/destock.h"                             
#include <signal.h>
//------------------------------------------------------ Name spaces
using namespace std;


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


// Tâche mère

int main()
// Algorithme :
// Allocation des ressources, initialisation et lancement des autres threads 

{
 
  // Initialisation des boites aux lettres
  Mailbox<Event>  balEvenements;
  Mailbox<Carton> balImprimante;
  Mailbox<Carton> balPalette;
  Mailbox<Palette> balStockage;
  Mailbox<Message> balMessages;
  Mailbox<Piece> balPiece;
  Mailbox<ListeCommandes> balCommandes; // tâche destock

  // Initialisation mémoires partagées

  // * shmem de la liste des lots :
  SharedMemoryLots lots;
  ListeLots listeLots;
  lots.content=&listeLots;

  // * shmem du stock :
  SharedMemoryStock stock;


  // Initialisation du générateur
  //srand(time(NULL));

  // Mutex
  Mutex  sortieStdMutex;
  Mutex  cartonPresent;

  sem_t  debutSyncro;
  sem_init(&debutSyncro, 0, 0);

  sem_t  finSerieMutex;
  sem_init(&finSerieMutex, 0, 0);
  sem_t  pauseSerieMutex;
  sem_init(&pauseSerieMutex, 0, 0);
  

  // Creation du gestionnaire de Log
  Log gestionnaireLog(sortieStdMutex);


  // Allocation des mutex et variables conditionnelles
  pthread_t 
    remplir_carton, 
    imprimer, 
    remplir_palette,
    stocker_palette,
    destocker_palette,
    controleur,
    serveur_reception,
    serveur_envoi,
    gestion_series;

  pthread_cond_t 
    condRC=PTHREAD_COND_INITIALIZER, // remplir carton
    condIMP=PTHREAD_COND_INITIALIZER, // imprimer
    condRP=PTHREAD_COND_INITIALIZER, // remplir palette
    condSP=PTHREAD_COND_INITIALIZER, // stocker
    condDP=PTHREAD_COND_INITIALIZER; // destocker
 
  pthread_mutex_t 
    condRCM=PTHREAD_MUTEX_INITIALIZER, // remplir carton
    condIMPM=PTHREAD_MUTEX_INITIALIZER, // imprimer
    condRPM=PTHREAD_MUTEX_INITIALIZER, // remplir palette
    condSPM=PTHREAD_MUTEX_INITIALIZER, // stocker
    condDPM=PTHREAD_MUTEX_INITIALIZER; // destocker


  // ===================================
  // ====== Creation des threads =======
  // ===================================

  // Création du thread remplir carton
  ArgRemplirCarton * argRC = new ArgRemplirCarton();
  argRC->pBalPieces=&balPiece;
  argRC->pBalCartons = &balImprimante;
  argRC->pBalEvenements = &balEvenements;
  argRC->mutCartonPresent=&cartonPresent;
  argRC->pCartonPresent = new bool(true);
  argRC->mutCv=&condRCM;
  argRC->cv=&condRC;
  argRC->nbLots=0;
  argRC->shMemLots=&lots;
  argRC->debutSyncro=&debutSyncro;
  argRC->finDeSerieMutex=&finSerieMutex;
  pthread_create (&remplir_carton, NULL, (void *(*)(void *))&remplirCarton, argRC);



  // Création du thread imprimer
  ArgImprimer * argImprimer = new ArgImprimer();
  argImprimer->eventBox= &balEvenements;
  argImprimer->balImprimante=&balImprimante;
  argImprimer->balPalette=&balPalette;
  argImprimer->varCond=&condIMP;
  argImprimer->mutex=&condIMPM;
  pthread_create (&imprimer, NULL, (void *(*)(void *)) &imprimer_thread, (void *)argImprimer);



  // Création du thread remplir palette
  ArgsRemplirPalette * argRP = new ArgsRemplirPalette();
  argRP->balImprimante=&balImprimante;
  argRP->balPalette=&balStockage;
  argRP->eventBox=&balEvenements;
  argRP->cw=&condRP;
  argRP->mxcw=&condRPM;
  argRP->shMemLots=&lots;
  pthread_create (&remplir_palette, NULL, (void *(*)(void *)) &remplirPalette_thread, argRP);

  

  //Creation du thread stocker palette
  ArgStock argStock;
  argStock.balStockage = &balStockage;
  argStock.balEvenements = &balEvenements;
  // argStock.reprise = ?; // reprise après erreur
  argStock.shMemLots = &lots;
  argStock.cv = &condSP;
  argStock.mutCv = &condSPM;
  argStock.stock = &stock;
  pthread_create (&stocker_palette, NULL, thread_stock, (void*) &argStock);


  //Création du thread destocker palette
  ArgDestock argDestock;
  argDestock.balEvenements = &balEvenements;
  argDestock.balCommandes = &balCommandes;
  argDestock.cv = &condDP;
  argDestock.mutCv = &condDPM;
  argDestock.stock = &stock;
  pthread_create (&destocker_palette, NULL, thread_destock, (void*) &argDestock);
  
  
  //Création du thread controleur
   
  ArgControleur * argControleur = new ArgControleur();
  argControleur->eventBox= &balEvenements;
  argControleur->gestionnaireLog=&gestionnaireLog;
  argControleur->balImprimante=&balImprimante;
  argControleur->balPalette=&balPalette;
  argControleur->balStockage=&balStockage;
  argControleur->balPiece=&balPiece;
  
  InfoThread remplirCarton;
  remplirCarton.id =remplir_carton;
  remplirCarton.cw=&condRC;
  remplirCarton.mx=&condRCM;
  argControleur->threads[REMPLIRCARTON]=remplirCarton;

  InfoThread remplirPalette;
  remplirPalette.id =remplir_palette;
  remplirPalette.cw=&condRP;
  remplirPalette.mx=&condRPM;
  argControleur->threads[REMPLIRPALETTE]=remplirPalette;

  InfoThread imprimerTI;
  imprimerTI.id =imprimer;
  imprimerTI.cw=&condIMP;
  imprimerTI.mx=&condIMPM;
  argControleur->threads[IMPRIMER]=imprimerTI;


  InfoThread stockerPalette;
  stockerPalette.id =stocker_palette;
  stockerPalette.cw=&condSP;
  stockerPalette.mx=&condSPM;
  argControleur->threads[STOCKERPALETTE]=stockerPalette;

  InfoThread destockerPalette;
  destockerPalette.id =destocker_palette;
  destockerPalette.cw=&condDP;
  destockerPalette.mx=&condDPM;
  argControleur->threads[DESTOCKERPALETTE]=destockerPalette;

  pthread_create (&controleur, NULL, (void *(*)(void *)) controleur_thread, (void *) argControleur);


  // Création du thread de reception(serveur)
  NetworkInitInfo * info = new NetworkInitInfo();
  info->netmb_ptr = &balMessages;
  info->socket_ptr = new int(0);
  info->shMemLots =&lots;
  info->debutSyncro = &debutSyncro;
  pthread_create (&serveur_reception, NULL, (void *(*)(void *)) thread_network, (void *)info);


  // Création du thread d'envoi(server)
  NetSendInitInfo * infoSend = new NetSendInitInfo();
  infoSend->netmb_ptr = &balMessages;
  infoSend->socket_ptr = new int(0);
  pthread_create (&serveur_envoi, NULL, (void *(*)(void *)) thread_netsend, (void *) infoSend);



  //Création du thread de gestion des séries
  ArgGestionSerie * gestionSerie = new ArgGestionSerie();
  gestionSerie->mtxPauseRequest=&pauseSerieMutex;
  gestionSerie->finDeSerie=&finSerieMutex;
  gestionSerie->eventBox=&balEvenements;
  pthread_create (&gestion_series, NULL, (void *(*)(void *)) gestionserie_thread, (void *) gestionSerie);


  cout<<"Phase moteur"<<endl;
  int a;
  cin>>a;
  cout<<endl<<endl<<endl;
  cout<<"Phase de destruction"<<endl;
  Event e(FINERREUR);
  balEvenements.Push(e,0);


  pthread_join(gestion_series, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(serveur_envoi, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(controleur, NULL);
  pthread_join(destocker_palette, NULL);
  pthread_join(stocker_palette, NULL);
  pthread_join(remplir_palette, NULL);
  pthread_join(imprimer, NULL);
  pthread_join(remplir_carton, NULL);


  delete infoSend->socket_ptr;

  delete infoSend;

  delete info;

  delete argControleur;

  delete argImprimer;

  delete argRC->pCartonPresent;
  
  delete argRC;

  return 0;
  
}
