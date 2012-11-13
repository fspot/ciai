#ifndef GESTIONSERIE_H
#define GESTIONSERIE_H

#include "../mailbox/mailbox.h"
#include "../multithreadObjects/mutex.h"
#include "../modeles/modeles.h"



struct ArgGestionSerie {
	sem_t* mtxPauseRequest;
	sem_t* finDeSerie;
	Mailbox<Event>* eventBox;
};

void gestionserie_thread(void* params);

#endif // GESTIONSERIE_H
