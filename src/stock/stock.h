
/*************************************************************************
                           Stock  -  Tâche qui gère le stockage.
*************************************************************************/

//---------- Interface de la tâche stock ---

#ifndef STOCK_H
#define STOCK_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 

struct ArgStock {
	Mailbox<Palette>* balStockage;
	Mailbox<Event>* balEvenements;
	sem_t* reprise; // reprise après erreur
	SharedMemoryLots *shMemLots;
	SharedMemoryStock *stock;
	// sem_t* sem_fin_de_serie;
	// bool* pCartonPresent;
	// Lot* lots;
	// unsigned int nbLots;
	// sem_t * finDeSerieMutex;
	pthread_cond_t* cv;
	pthread_mutex_t* mutCv;
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void *thread_stock(void *argStock);

#endif