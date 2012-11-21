#include <pthread.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/imprimer/imprimer.h"
#include "../../src/log/log.h"

#define MAXCARTON 15
#define MAXCARTONPANNE 1000

using namespace std;

// creation du Log
static Mutex mtxStandardOutput;
static Log gestionnaireLog(mtxStandardOutput);

// creation varCond et mutex pour attente reprise
static pthread_cond_t varCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Fontions pour tests
void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

void depotCartonBalImprimante(ArgImprimer * args, int numCarton, int maxCarton) {
	Carton carton;
	carton.id = numCarton;
	carton.lot = NULL;
	carton.nbrebut = 0;
	if (numCarton == maxCarton)
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
		depotCartonBalImprimante( args, i, MAXCARTON );
		retraitCartonBalPalette(args);
	}
}

void simuFilePleine (ArgImprimer * args) {
	ecriture_log_mere(&gestionnaireLog,"SIMULATION FILE PLEINE",EVENT);
	for (int i=1; i<=MAXCARTON; i++) {
		depotCartonBalImprimante( args, i, MAXCARTON );
	}
}
 void viderFilePleine(ArgImprimer * args) {
	for (int i=1; i<=10; i++) {
		retraitCartonBalPalette( args );
	}
	pthread_cond_signal(&varCond);
}

void simuPanneImprim(ArgImprimer * args) {
	ecriture_log_mere(&gestionnaireLog,"SIMULATION PANNE IMPRIMANTE",EVENT);
	for (int i=1; i<=MAXCARTONPANNE; i++) {
		depotCartonBalImprimante( args, i, MAXCARTONPANNE );
		retraitCartonBalPalette(args);
	}
}

int main() {

	// Initialisation des boites aux lettres
	Mailbox<Event>  balEvenements;
	Mailbox<Carton> balImprimante;
	Mailbox<Carton> balPalette;
	
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
	
	int choix;
	cout << "Choix du test : " << endl;
	cout << "Test 1 : Fonctionnement normal\n"
		<< "Test 2 : Remplissage de la file d'attente puis vidage avec reprise de la tache\n"
		<< "Test 3 : Simulation d'une panne imprimante\n" << "Votre choix :" << endl;
	cin >> choix;

	if (choix==1) {
		//Test 1 : Fonctionnement normal
		simuFonctionnementNormal(argImprimer);
	}	
	else if (choix==2) {
		//Test 2 : Remplissage de la file d'attente puis vidage avec reprise de la tache
		simuFilePleine(argImprimer);
		sleep(1);
		viderFilePleine(argImprimer);
	}
	else if (choix==3) {
		//Test 3 : Simulation d'une panne imprimante
		simuPanneImprim(argImprimer);
	}
	 
	pthread_join(imprimer, NULL);
	
	ecriture_log_mere(&gestionnaireLog,"Fin de la tâche mère",EVENT);

	return 0;

}
