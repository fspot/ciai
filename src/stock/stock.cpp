#include <iostream>

#include "stock.h"
#include "pthread.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"

using namespace std; 


void *thread_stock(void *argStock)
{
	cout << "Thread Stock launched" << endl;

	// ==== unpack arguments
	ArgStock *infos = (ArgStock*) argStock;
	Mailbox<Palette>* balStockage = argStock->balStockage;
	Mailbox<Event>* balEvenements = argStock->balEvenements;
	sem_t* reprise = argStock->reprise;
	pthread_cond_t* cv  = argStock->cv;
	pthread_mutex_t* mutCv = argStock->mutCv;
	SharedMemoryLots *shMemLots = argStock->shMemLots;
	// stock ?

	// ==== semaphore wait (synchronisation du lancement des threads moteurs)
	// SEM_WAIT()

	Palette p;
	int lot = 0, pal = 0; // num lot actuel, num palette actuelle (au sein du lot)

	while(lot < shMemLots->content->tot)
		p = balStockage->Pull(); // opération qui peut être bloquante : réception d'une palette à stocker.

		if (p.fin) // test cas spécial
			break;

		// "eventuellement" on peut verifier si la palette correspond..

		// ==== phase de stockage
		// prise du mutex du stock
		// stock[nomLot]++; // stockage 
		// rendu du mutex du stock

		pal++;
		if (pal == shMemLots->content->lots.palettes) {
			lot++; // passage au lot suivant
			pal = 0;
		}
	}

	pthread_exit(0);
}