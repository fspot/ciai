/*************************************************************************
                           mothertask.cpp  -  Tâche mère
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche mère ---

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
#include "sockets/network.h"
#include "sockets/netsend.h"
#include "imprimer/imprimer.h"
#include "piece/piece.h"
#include "log/log.h"
#include "remplirCarton/remplirCarton.h"
#include "remplirPalette/remplirPalette.h"
#include "stock/stock.h"
#include "destock/destock.h"                             
//------------------------------------------------------ Name spaces
using namespace std;


// ----------------------------------------------------- Statics :
pthread_t 
genere_piece,
    remplir_carton, 
    imprimer, 
    remplir_palette,
    stocker_palette,
    destocker_palette,
    controleur,
    serveur_reception,
    serveur_envoi;

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

//Méthode d'écriture dans le log
void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
#ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
#else
    unGestionnaire->Write(msg,unType,false);
#endif 
}

//Handler appelé en cas de signal envoyé a la tache mère
void catcher(int noSignal)
{
    pthread_cancel(genere_piece);
    pthread_cancel(remplir_carton);
    pthread_cancel(imprimer);
    pthread_cancel(remplir_palette);
    pthread_cancel(stocker_palette);
    pthread_cancel(destocker_palette);
    pthread_cancel(controleur);
    pthread_cancel(serveur_reception);
    pthread_cancel(serveur_envoi);
    cerr << "Fermeture forcée" << endl;
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

    ecriture_log_mere(&gestionnaireLog,"Lancement de la tâche mère",EVENT);
  
// Variables conditionelles et leur mutex de protection associé
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

    // initialisation de attr et param pour les threads temps réel :
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sched_param param;
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_getschedparam(&attr, &param);


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
    argRC.mutCv=&condRCM;
    argRC.cv=&condRC;
    argRC.capteurCarton=&stubAbscenceCarton;

    //gestion du lot courant pour la simulation
    argRC.lotCourant=&lotCourant;
    argRC.lotCourantMutex=&memLotCourant;

    argRC.shMemLots=&lots;
    argRC.debutSyncro=&debutSyncro;

    // thread temps réel : priorité 20
    param.sched_priority = 20;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&remplir_carton, &attr, (void *(*)(void *))&remplirCarton, (void *)&argRC);



    // Création du thread imprimer
    ArgImprimer  argImprimer;
    argImprimer.gestionnaireLog=&gestionnaireLog;
    argImprimer.eventBox= &balEvenements;
    argImprimer.balImprimante=&balImprimante;
    argImprimer.balPalette=&balPalette;
    argImprimer.varCond=&condIMP;
    argImprimer.mutex=&condIMPM;
    argImprimer.capteurPanne=&stubPanneImprimante;

    // thread temps réel : priorité 30
    param.sched_priority = 30;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&imprimer, &attr, (void *(*)(void *)) &imprimer_thread, (void *)&argImprimer);



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

    // Méthodes utiles pour la simulation
    argRP.capteurPalette=stubPresencePalette;
    argRP.capteurEmbalage=stubErrEmbalagePalette;

    // thread temps réel : priorité 40
    param.sched_priority = 40;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&remplir_palette, &attr, (void *(*)(void *)) &remplirPalette_thread, (void *)&argRP);

  
    //Creation du thread stocker palette
    ArgStock argStock;
    argStock.gestionnaireLog=&gestionnaireLog;
    argStock.balStockage = &balStockage;
    argStock.balEvenements = &balEvenements;
    argStock.shMemLots = &lots;
    argStock.cv = &condSP;
    argStock.mutCv = &condSPM;
    argStock.stock = &stock;

    // thread temps réel : priorité 50
    param.sched_priority = 50;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&stocker_palette, &attr, thread_stock, (void*) &argStock);


    //Création du thread destocker palette
    ArgDestock argDestock;
    argDestock.gestionnaireLog=&gestionnaireLog;
    argDestock.balEvenements = &balEvenements;
    argDestock.balCommandes = &balCommandes;
    argDestock.cv = &condDP;
    argDestock.mutCv = &condDPM;
    argDestock.stock = &stock;
    argDestock.balMessages = &balMessages;

    // thread temps partagé ou prio 70 ?
    pthread_create (&destocker_palette, NULL, thread_destock, (void*) &argDestock);


    // Création du thread genere_piece (Thread de simulation)
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

    // thread temps réel : priorité 10
    param.sched_priority = 10;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&genere_piece, &attr, thread_piece, (void*) &argPiece);
  
  
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

    // thread temps réel : priorité 80
    param.sched_priority = 80;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&controleur, &attr, (void *(*)(void *)) controleur_thread, (void *) &argControleur);


    // Création du thread de reception(serveur)
    NetworkInitInfo  info;
    info.gestionnaireLog=&gestionnaireLog;
    info.balEvenements=&balEvenements;
    info.balCommandes = &balCommandes;
    info.netmb_ptr = &balMessages;
    info.socket_ptr = &socket_ptr;
    info.shMemLots =&lots;
    info.debutSyncro = &debutSyncro;

    // thread tps partagé ou prio 60 ?
    pthread_create (&serveur_reception, NULL, (void *(*)(void *)) thread_network, (void *)&info);


    // Création du thread d'envoi(server)
    NetSendInitInfo  infoSend ;
    infoSend.gestionnaireLog=&gestionnaireLog;
    infoSend.netmb_ptr = &balMessages;
    infoSend.socket_ptr = &socket_ptr;

    // thread prio 85 
    param.sched_priority = 85;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&serveur_envoi, &attr, (void *(*)(void *)) thread_netsend, (void *) &infoSend);


    // ===========================
    // ==== FIN THREAD CREATE ====
    // ===========================

  

    // Mise en place du handler pour l'arret forcé
    signal(SIGINT,catcher);

    ecriture_log_mere(&gestionnaireLog,"Phase moteur - tache mere",EVENT);
  
  
    pthread_join(controleur, NULL);
    pthread_join(destocker_palette, NULL);
    pthread_join(stocker_palette, NULL);
    pthread_join(remplir_palette, NULL);
    pthread_join(imprimer, NULL);
    pthread_join(remplir_carton, NULL);
    pthread_join(serveur_envoi, NULL);
    pthread_cancel(serveur_reception);
  
    ecriture_log_mere(&gestionnaireLog,"Fin de la tâche mère",EVENT);
    sem_destroy(&debutSyncro);


    sem_destroy(&pauseSerieMutex);
    return 0;
  
}
