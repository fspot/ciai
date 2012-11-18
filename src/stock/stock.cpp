#include <iostream>
#include <map>

#include "pthread.h"

#include "stock.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"

using namespace std; 

void ecriture_log_stock(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


void * thread_stock(void *argStock)
{

	// ==== unpack arguments
	ArgStock *infos = (ArgStock*) argStock;
        ecriture_log_stock(infos->gestionnaireLog,"Lancement de la tache de stock",EVENT);
	Mailbox<Palette>* balStockage = infos->balStockage;
	Mailbox<Event>* balEvenements = infos->balEvenements;
	sem_t* reprise = infos->reprise;
	pthread_cond_t* cv  = infos->cv;
	pthread_mutex_t* mutCv = infos->mutCv;
	SharedMemoryLots *shMemLots = infos->shMemLots;
	SharedMemoryStock *shMemStock = infos->stock;
	// stock ?
	Palette p;
	int lot = 0, pal = 0; // num lot actuel, num palette actuelle (au sein du lot)

	while(1)
	{
		p = balStockage->Pull(); // opération qui peut être bloquante : réception d'une palette à stocker.
        	ecriture_log_stock(infos->gestionnaireLog,"Une palette arrivee - tache stock",EVENT);
		if (p.fin) // test cas spécial
		{
        		ecriture_log_stock(infos->gestionnaireLog,"Fin de la tache stock",EVENT);
			pthread_exit(0);
		}
		// "eventuellement" on peut verifier si la palette correspond..

		// ==== phase de stockage
		shMemStock->mutex.lock();
		string nomLot = shMemLots->content->lots[lot].nom;
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
			{
        			ecriture_log_stock(infos->gestionnaireLog,"Fin de la tache stock ",EVENT);
				pthread_exit(0);
			}
		}
	}

	pthread_exit(0);
}
