#include <pthread.h>
#include <iostream>
#include <string>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/imprimer/imprimer.h"
#include "../../src/log/log.h"

#define MAXCARTON 15

using namespace std;

// creation du Log
static Mutex mtxStandardOutput;
static Log gestionnaireLog(mtxStandardOutput);

void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType);                                                              
void depotCartonBalImprimante(ArgImprimer * args, int numCarton);
void retraitCartonBalPalette(ArgImprimer * args);
void simuFonctionnementNormal(ArgImprimer * args);
void simuFilePleine (ArgImprimer * args);

int main() {

	// Initialisation des boites aux lettres
	Mailbox<Event>  balEvenements;
	Mailbox<Carton> balImprimante;
	Mailbox<Carton> balPalette;
	
	// creation varCond et mutex pour attente reprise
	pthread_cond_t varCond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	//Creation du thread imprimer
	pthread_t imprimer;

	ArgImprimer * argImprimer = new ArgImprimer;
	argImprimer->eventBox = &balEvenements;
	argImprimer->balImprimante = &balImprimante;
	argImprimer->balPalette = &balPalette;
	argImprimer->varCond = &varCond;
	argImprimer->mutex = &mutex;
	argImprimer->gestionnaireLog = &gestionnaireLog;
	pthread_create (&imprimer, NULL,(void *(*) (void *)) &imprimer_thread, argImprimer);
	
	ecriture_log_mere(&gestionnaireLog,"Phase moteur - tache mere",EVENT);

	//simuFonctionnementNormal(argImprimer);	

	simuFilePleine(argImprimer);

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

void depotCartonBalImprimante(ArgImprimer * args, int numCarton) {
	Carton carton;
	carton.id = numCarton;
	carton.lot = NULL;
	carton.nbrebut = 0;
	if (numCarton == MAXCARTON)
		carton.fin = true;
	else
		carton.fin = false;
	args->balImprimante->Push(carton, 1);
}

void retraitCartonBalPalette(ArgImprimer * args) {
	Carton carton;
	carton = args->balPalette->Pull();
} 

void simuFonctionnementNormal(ArgImprimer * args) {
	ecriture_log_mere(&gestionnaireLog,"SIMULATION FONCTIONNEMENT NORMAL",EVENT);
	for (int i=1; i<=MAXCARTON; i++) {
		depotCartonBalImprimante( args, i );
		retraitCartonBalPalette(args);
	}
}

void simuFilePleine (ArgImprimer * args) {
	ecriture_log_mere(&gestionnaireLog,"SIMULATION FILE PLEINE",EVENT);
	for (int i=1; i<=MAXCARTON; i++) {
		depotCartonBalImprimante( args, i );
	}
}

