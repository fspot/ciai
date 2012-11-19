/*************************************************************************
  Destock  -  Tâche qui gère le déstockage à partir des commandes reçues.
*************************************************************************/

//---------- Interface de la tâche destock ---

#ifndef DESTOCK_H
#define DESTOCK_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 

struct ArgDestock {
	SharedMemoryStock *stock;
	Mailbox<Event>* balEvenements;
	Mailbox<ListeCommandes>* balCommandes;
	Mailbox<Message>* balMessages;
  	Log * gestionnaireLog;
	pthread_cond_t* cv;
	pthread_mutex_t* mutCv;
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void *thread_destock(void *argDestock);

#endif
