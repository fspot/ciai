#include <iostream>
#include <map>

#include "pthread.h"

#include "stock.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"

using namespace std; 



void * thread_stock(void *argStock)
{
	cout << "thread stock launched." << endl;
	// ==== unpack arguments
	ArgStock *infos = (ArgStock*) argStock;
	Mailbox<Palette>* balStockage = infos->balStockage;
	Mailbox<Event>* balEvenements = infos->balEvenements;
	sem_t* reprise = infos->reprise;
	pthread_cond_t* cv  = infos->cv;
	pthread_mutex_t* mutCv = infos->mutCv;
	SharedMemoryLots *shMemLots = infos->shMemLots;
	SharedMemoryStock *shMemStock = infos->stock;
	// stock ?

	// ==== semaphore wait (synchronisation du lancement des threads moteurs)
	sem_wait(infos->debutSyncro);

	Palette p;
	int lot = 0, pal = 0; // num lot actuel, num palette actuelle (au sein du lot)

	while(1)
	{
		p = balStockage->Pull(); // opération qui peut être bloquante : réception d'une palette à stocker.

		if (p.fin) // test cas spécial
			break;

		// "eventuellement" on peut verifier si la palette correspond..

		// ==== phase de stockage
		string nomLot = shMemLots->content->lots[lot].nom;
		shMemStock->mutex.lock();
		shMemStock->stock[nomLot]++;
		shMemStock->mutex.unlock();

		// passage à la palette suivante :
		pal++;
		if (pal == shMemLots->content->lots[lot].palettes) {
			// passage au lot suivant :
			pal = 0;
			lot++;

			// test de fin de production :
			if (lot == shMemLots->content->lots.size())
				break;
		}
	}

	pthread_exit(0);
}
