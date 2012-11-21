
#include <pthread.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/imprimer/imprimer.h"
#include "../../src/log/log.h"

using namespace std;

// creation du Log
static Mutex mtxStandardOutput;
static Log gestionnaireLog(mtxStandardOutput);

// creation varCond et mutex pour attente reprise
static pthread_cond_t varCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Fontions pour tests
void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType);
void depotCartonBalImprimante(ArgImprimer * args, int numCarton);
void retraitCartonBalPalette(ArgImprimer * args);
void simuFonctionnementNormal(ArgImprimer * args);


int main() {

    pthread_cond_t condSP=PTHREAD_COND_INITIALIZER;
    pthread_mutex_t condSPM=PTHREAD_MUTEX_INITIALIZER;

    // Initialisation des boites aux lettres
    Mailbox<Event>  balEvenements;
    Mailbox<Carton> balStockage;

    // * shmem de la liste des lots :
    SharedMemoryLots lots;
    ListeLots listeLots;
    lots.content=&listeLots;

    // * shmem du stock :
    SharedMemoryStock stock;

    //Creation du thread imprimer
    pthread_t imprimer;

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

    // thread temps réel : priorité 50
    param.sched_priority = 50;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create (&stocker_palette, &attr, thread_stock, (void*) &argStock);

    simuFilePleine(argImprimer);
    sleep(1);
    viderFilePleine(argImprimer);

    pthread_join(imprimer, NULL);

    ecriture_log_mere(&gestionnaireLog,"Fin de la tâche mère",EVENT);

    return 0;

}

void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType)
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif
}
