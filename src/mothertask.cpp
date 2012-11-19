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
#include "piece/piece.h"
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

void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

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

  // socket
  int socket_ptr;

  // Initialisation du générateur
  //srand(time(NULL));

  // Mutex
  Mutex  sortieStdMutex;
  Mutex  cartonPresent;
  Mutex  clapet;

  Mutex memLotCourant;

  sem_t  debutSyncro;
  sem_init(&debutSyncro, 0, 0);

//  sem_t  finSerieMutex;
//  sem_init(&finSerieMutex, 0, 0);
  sem_t  pauseSerieMutex;
  sem_init(&pauseSerieMutex, 0, 0);
  

  // Creation du gestionnaire de Log
  Log gestionnaireLog(sortieStdMutex);

  ecriture_log_mere(&gestionnaireLog,"Lancement de la tâche mère",EVENT);


  // Allocation des mutex et variables conditionnelles
  pthread_t 
    genere_piece,
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


  // Variables necessaires à la simulation


  int lotCourant=0;
  // ===================================
  // ====== Creation des threads =======
  // ===================================

  // Création du thread remplir carton
  ArgRemplirCarton  argRC;
  argRC.pBalPieces=&balPiece;
  argRC.pBalCartons = &balImprimante;
  argRC.pBalEvenements = &balEvenements;
  argRC.pBalMessages = &balMessages;
  argRC.gestionnaireLog=&gestionnaireLog;
  argRC.mutCartonPresent=&cartonPresent;
  argRC.pCartonPresent = new bool(true);
  argRC.mutCv=&condRCM;
  argRC.cv=&condRC;

  //gestion du lot courant pour simul 
  argRC.lotCourant=&lotCourant;
  argRC.lotCourantMutex=&memLotCourant;

  argRC.shMemLots=&lots;
  argRC.debutSyncro=&debutSyncro;
//  argRC.finDeSerieMutex=&finSerieMutex;
  pthread_create (&remplir_carton, NULL, (void *(*)(void *))&remplirCarton, (void *)&argRC);



  // Création du thread imprimer
  ArgImprimer  argImprimer;
  argImprimer.gestionnaireLog=&gestionnaireLog;
  argImprimer.eventBox= &balEvenements;
  argImprimer.balImprimante=&balImprimante;
  argImprimer.balPalette=&balPalette;
  argImprimer.varCond=&condIMP;
  argImprimer.mutex=&condIMPM;
  pthread_create (&imprimer, NULL, (void *(*)(void *)) &imprimer_thread, (void *)&argImprimer);



  // Création du thread remplir palette
  ArgsRemplirPalette  argRP;
  argRP.gestionnaireLog=&gestionnaireLog;
  argRP.balPalette=&balPalette;
  argRP.balStockage=&balStockage;
  argRP.eventBox=&balEvenements;
  argRP.balMessages = &balMessages;
  argRP.cw=&condRP;
  argRP.mxcw=&condRPM;
  argRP.shMemLots=&lots;
  argRP.debutSyncro=&debutSyncro;
  pthread_create (&remplir_palette, NULL, (void *(*)(void *)) &remplirPalette_thread, (void *)&argRP);

  

  //Creation du thread stocker palette
  ArgStock argStock;
  argStock.gestionnaireLog=&gestionnaireLog;
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
  argDestock.gestionnaireLog=&gestionnaireLog;
  argDestock.balEvenements = &balEvenements;
  argDestock.balCommandes = &balCommandes;
  argDestock.cv = &condDP;
  argDestock.mutCv = &condDPM;
  argDestock.stock = &stock;
  argDestock.balMessages = &balMessages;
  pthread_create (&destocker_palette, NULL, thread_destock, (void*) &argDestock);


  // Création du thread genere_piece
  ArgPiece  argPiece;
  argPiece.balPiece =&balPiece;
  argPiece.balEvenements = &balEvenements;
  argPiece.gestionnaireLog = &gestionnaireLog;
  argPiece.clapet = &clapet;
  argPiece.debutSyncro = &debutSyncro;
  argPiece.shMemLots = &lots;
  //gestion du lot courant pour simul 
  argPiece.lotCourant=&lotCourant;
  argPiece.lotCourantMutex=&memLotCourant;
  pthread_create (&genere_piece, NULL, thread_piece, (void*) &argPiece);
  
  
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

  
  InfoThread remplirCarton;
  remplirCarton.id =remplir_carton;
  remplirCarton.cw=&condRC;
  remplirCarton.mx=&condRCM;
  argControleur.threads[REMPLIRCARTON]=remplirCarton;

  InfoThread remplirPalette;
  remplirPalette.id =remplir_palette;
  remplirPalette.cw=&condRP;
  remplirPalette.mx=&condRPM;
  argControleur.threads[REMPLIRPALETTE]=remplirPalette;

  InfoThread imprimerTI;
  imprimerTI.id =imprimer;
  imprimerTI.cw=&condIMP;
  imprimerTI.mx=&condIMPM;
  argControleur.threads[IMPRIMER]=imprimerTI;

  InfoThread stockerPalette;
  stockerPalette.id =stocker_palette;
  stockerPalette.cw=&condSP;
  stockerPalette.mx=&condSPM;
  argControleur.threads[STOCKERPALETTE]=stockerPalette;

  InfoThread destockerPalette;
  destockerPalette.id = destocker_palette;
  destockerPalette.cw = &condDP;
  destockerPalette.mx = &condDPM;
  argControleur.threads[DESTOCKERPALETTE]=destockerPalette;

  pthread_create (&controleur, NULL, (void *(*)(void *)) controleur_thread, (void *) &argControleur);


  // Création du thread de reception(serveur)
  NetworkInitInfo  info;
  info.gestionnaireLog=&gestionnaireLog;
  info.balEvenements=&balEvenements;
  info.netmb_ptr = &balMessages;
  info.socket_ptr = &socket_ptr;
  info.shMemLots =&lots;
  info.debutSyncro = &debutSyncro;
  pthread_create (&serveur_reception, NULL, (void *(*)(void *)) thread_network, (void *)&info);


  // Création du thread d'envoi(server)
  NetSendInitInfo  infoSend ;
  infoSend.gestionnaireLog=&gestionnaireLog;
  infoSend.netmb_ptr = &balMessages;
  infoSend.socket_ptr = &socket_ptr;
  pthread_create (&serveur_envoi, NULL, (void *(*)(void *)) thread_netsend, (void *) &infoSend);



  ecriture_log_mere(&gestionnaireLog,"Phase moteur - tache mere",EVENT);


  
  pthread_join(controleur, NULL);
  pthread_join(destocker_palette, NULL);
  pthread_join(stocker_palette, NULL);
  pthread_join(remplir_palette, NULL);
  pthread_join(imprimer, NULL);
  pthread_join(remplir_carton, NULL);
  pthread_join(serveur_envoi, NULL);
  pthread_cancel(serveur_reception);

  delete argRC.pCartonPresent;
  
  ecriture_log_mere(&gestionnaireLog,"Fin de la tâche mère",EVENT);
  return 0;
  
}
