#ifndef GESTIONSERIE_H
#define GESTIONSERIE_H

#include "../mailbox/mailbox.h"
#include "../multithreadObjects/mutex.h"
#include "../modeles/modeles.h"
#include "../log/log.h" 



struct ArgGestionSerie {
	sem_t* mtxPauseRequest;
	sem_t* finDeSerie;
  	Log * gestionnaireLog;
	Mailbox<Event>* eventBox;
};

void gestionserie_thread(void* params);

#endif // GESTIONSERIE_H
