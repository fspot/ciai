/*************************************************************************
                           stock  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche stock

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <map>
#include "pthread.h"


//------------------------------------------------------ Include personnel
#include "stock.h"
#include <mailbox/mailbox.h>
#include <modeles/modeles.h>
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

// Méthode d'écriture dans le log
void ecriture_log_stock(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

//Fonction du thread stock
void * thread_stock(void *argStock)
{

	// Récupération des arguments et conversion
	ArgStock *infos = (ArgStock*) argStock;
        ecriture_log_stock(infos->gestionnaireLog,"Lancement de la tache de stock",EVENT);
	Mailbox<Palette>* balStockage = infos->balStockage;
	Mailbox<Event>* balEvenements = infos->balEvenements;
	sem_t* reprise = infos->reprise;
	pthread_cond_t* cv  = infos->cv;
	pthread_mutex_t* mutCv = infos->mutCv;
	SharedMemoryLots *shMemLots = infos->shMemLots;
	SharedMemoryStock *shMemStock = infos->stock;
	Palette p;
	int lot = 0, pal = 0; // numéro de lot actuel, num palette actuelle (au sein du lot)

	while(1)
	{
		p = balStockage->Pull(); // opération qui peut être bloquante : réception d'une palette à stocker.

		if (p.fin) // test cas spécial
		{
        		ecriture_log_stock(infos->gestionnaireLog,"Fin de la tache stock",EVENT);
			pthread_exit(0);
		}
        	ecriture_log_stock(infos->gestionnaireLog,"La tache de stockage a recu une palette",EVENT);
		// "eventuellement" on peut verifier si la palette correspond..

		// ==== phase de stockage
		shMemStock->mutex.lock();
		string nomLot = shMemLots->content->lots[lot].nom;
		shMemStock->stock[nomLot]++;
		shMemStock->mutex.unlock();
        	ecriture_log_stock(infos->gestionnaireLog,"La tache de stockage a stocké cette palette",EVENT);
		// passage à la palette suivante :
		pal++;
		if (pal == shMemLots->content->lots[lot].palettes) {
			// passage au lot suivant :
			pal = 0;
			lot++;

			// test de fin de production :
			if (lot == shMemLots->content->lots.size())
			{
				balEvenements->Push(Event(FIN),1);
        			ecriture_log_stock(infos->gestionnaireLog,"Fin de la tache stock ",EVENT);
				pthread_exit(0);
			}
		}
	}
}
