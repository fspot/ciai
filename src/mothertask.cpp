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
#include "multithreadObjects/sharedMemory.h"
#include "mailbox/mailbox.h"
#include "controleur/controleur.h"
#include "sockets/network.h"
#include "sockets/netsend.h"
#include "tacheImprimer/imprimer.h"
#include "log/log.h"
#include "remplissageCarton/remplissageCarton.h"
//------------------------------------------------------ Name spaces
using namespace std;


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

// Fonctions temporaires
void * remplir_palette_function()
{
  cout<<"\t\tTâche remplir palette lancée"<<endl;
  pthread_exit(0);
}
void * stocker_palette_function()
{
  cout<<"\t\tTâche stocker palette lancée"<<endl;
  pthread_exit(0);
}
void * destocker_palette_function()
{
  cout<<"\t\tTâche destocker palette lancée"<<endl;
  pthread_exit(0);
}

void * gestion_series_function()
{
  cout<<"\t\tTâche gestion series lancée"<<endl;
  pthread_exit(0);
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

  // Initialisation mémoires partagées
  SharedMemory series;
  series.mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&series.mutex,NULL);


  SharedMemory infos;
  infos.mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&infos.mutex,NULL);


  // Mutex
  Mutex  sortieStdMutex;
  

  // Creation du gestionnaire de Log
  Log gestionnaireLog(sortieStdMutex);


  // Allocation des mutex et variables conditionnelles
  pthread_t remplir_carton, imprimer, remplir_palette,stocker_palette,destocker_palette,controleur,serveur_reception,serveur_envoi,gestion_series;
  pthread_cond_t condRC=PTHREAD_COND_INITIALIZER,
    condIMP=PTHREAD_COND_INITIALIZER,
    condRP=PTHREAD_COND_INITIALIZER,
    condSP=PTHREAD_COND_INITIALIZER,
    condDP=PTHREAD_COND_INITIALIZER;
 

  pthread_mutex_t condRCM=PTHREAD_MUTEX_INITIALIZER,
    condIMPM=PTHREAD_MUTEX_INITIALIZER,
    condRPM=PTHREAD_MUTEX_INITIALIZER,
    condSPM=PTHREAD_MUTEX_INITIALIZER,
    condDPM=PTHREAD_MUTEX_INITIALIZER;


  //Creation des threads

  // Création du thread remplissage carton
  ArgRemplissageCarton * argRC = new ArgRemplissageCarton();
  argRC->pBalPieces=&balPiece;
  argRC->pBalCartons = &balImprimante;
  argRC->pBalEvenements = &balEvenements;
  argRC->mutCartonPresent=NULL;
  argRC->pCartonPresent = new bool(true);
  argRC->mutCv=&condRCM;
  argRC->cv=&condRC;
  argRC->nbLots=0;
  pthread_create (&remplir_carton, NULL, (void *)&remplirCarton, argRC);



  // Création du thread imprimer
  ArgImprimer * argImprimer = new ArgImprimer();
  argImprimer->eventBox= &balEvenements;
  argImprimer->balImprimante=&balImprimante;
  argImprimer->balPalette=&balPalette;
  argImprimer->varCond=&condIMP;
  argImprimer->mutex=&condIMPM;
  pthread_create (&imprimer, NULL, (void *) &imprimer_thread, (void *)argImprimer);


  // Création du thread remplir palette
  pthread_create (&remplir_palette, NULL, (void *) &remplir_palette_function, NULL);

  //Creation du thread stocker palette
  pthread_create (&stocker_palette, NULL, (void *) &stocker_palette_function, NULL);

  //Création du thread destocker palette
  pthread_create (&destocker_palette, NULL, (void *) destocker_palette_function, NULL);
  
  //Création du thread controleur
   
  ArgControleur * argControleur = new ArgControleur();
  argControleur->eventBox= &balEvenements;
  argControleur->gestionnaireLog=&gestionnaireLog;
  
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

  pthread_create (&controleur, NULL, (void *) controleur_thread, (void *) argControleur);


  // Création du thread de reception(serveur)
  NetworkInitInfo * info = new NetworkInitInfo();
  info->netmb_ptr = &balMessages;
  info->socket_ptr = new int(0);
  pthread_create (&serveur_reception, NULL, (void *) thread_network, (void *)info);


  // Création du thread d'envoi(server)
  NetSendInitInfo * infoSend = new NetSendInitInfo();
  infoSend->netmb_ptr = &balMessages;
  infoSend->socket_ptr = new int(0);
  pthread_create (&serveur_envoi, NULL, (void *) thread_netsend, (void *) infoSend);



  //Création du thread de gestion des séries
  pthread_create (&gestion_series, NULL, (void *) gestion_series_function, (void *) NULL);



  cout<<"Phase moteur"<<endl;
  int a;
  cin>>a;
  cout<<endl<<endl<<endl;
  cout<<"Phase de destruction"<<endl;


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
  cout<<"\tThread détruits"<<endl;

  return 0;
  
}
