#ifndef GESTIONSERIE_H
#define GESTIONSERIE_H

#include "../mailbox/mailbox.h"
#include "../multithreadObjects/mutex.h"
#include "../modeles/modeles.h"



struct ArgGestionSerie {
	Mutex* mtxPauseRequest;
	Mutex* mtxSerieEnCours;
	Mailbox<Event>* eventBox;
};

void gestionserie_thread(void* params);

#endif // GESTIONSERIE_H
