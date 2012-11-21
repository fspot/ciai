
#include <pthread.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/stock/stock.h"
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

int main() {

    pthread_cond_t condSP=PTHREAD_COND_INITIALIZER;
    pthread_mutex_t condSPM=PTHREAD_MUTEX_INITIALIZER;
	
    // initialisation de attr et param pour les threads temps réel :
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sched_param param;
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_getschedparam(&attr, &param);

    // Initialisation des boites aux lettres
    Mailbox<Event>  balEvenements;
    Mailbox<Palette> balStockage;

    // * shmem de la liste des lots :
    SharedMemoryLots lots;
    ListeLots listeLots;
    lots.content=&listeLots;

    // * shmem du stock :
    SharedMemoryStock stock;

    //Creation du thread stocker_palette
    pthread_t stocker_palette;

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
	
    //placement des cartons dans la bal
    int nbPalettes=1;
    for(int i =0;i<nbPalettes;i++)
    {
	Palette palette;
	palette.id=i;
	Lot lot;
	lot.nom="lot_de_test";
	lot.pieces=100;
	lot.cartons=100;
	lot.palettes=100;
	lot.rebut=100;
	lot.dim[0]=100;
	lot.dim[1]=100;
	lot.dim[2]=100;
	palette.lot=&lot;
	if(i==nbPalettes-1)
	{
	    palette.fin=true;
	}
	palette.fin=false;
		
	balStockage.Push(palette);
    }
    sleep(1);

    // thread temps réel : priorité 50
    param.sched_priority = 50;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
	
    cout<<"5"<<endl;
    pthread_create (&stocker_palette, &attr, thread_stock, (void*) &argStock);

    cout<<"6"<<endl;

    pthread_join(stocker_palette, NULL);

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
