#include <pthread.h>
#include <iostream>
#include <string>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/imprimer/imprimer.h"
#include "../../src/log/log.h"


using namespace std;

void depotCartonBalImprimante(ArgImprimer * args, int numCarton) {
	Carton carton;
	carton.id = numCarton;
	carton.lot = NULL;
	carton.nbrebut = 0;
	
	args->balImprimante->Push(carton, 1);
	sleep(1);
}

void retraitCartonBalPalette(ArgImprimer * args) {
	Carton carton;
	carton = args->balPalette->Pull();
} 

void simuFonctionnementNormal(ArgImprimer * args) {
	for (int i=1; i<15; i++) {
		depotCartonBalImprimante( args, i );
		retraitCartonBalPalette(args);
	}
}

void simuFilePleine (ArgImprimer * args) {
	for (int i=1; i<15; i++) {
		depotCartonBalImprimante( args, i );
	}
}

int main() {

	// Initialisation des boites aux lettres
	Mailbox<Event>  balEvenements;
	Mailbox<Carton> balImprimante;
	Mailbox<Carton> balPalette;
	
	// creation varCond et mutex pour attente reprise
	pthread_cond_t varCond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	// creation du Log
	Mutex mtxStandardOutput;
	Log SgestionnaireLog(mtxStandardOutput);

	//Creation du thread imprimer
	pthread_t imprimer;

	ArgImprimer * argImprimer = new ArgImprimer;
	argImprimer->eventBox = &balEvenements;
	argImprimer->balImprimante = &balImprimante;
	argImprimer->balPalette = &balPalette;
	argImprimer->varCond = &varCond;
	argImprimer->mutex = &mutex;
	pthread_create (&imprimer, NULL,(void *(*) (void *)) &imprimer_thread, argImprimer);
	
	simuFonctionnementNormal(argImprimer);	

	simuFilePleine(argImprimer);

	pthread_join(imprimer, NULL);

	return 0;

}
