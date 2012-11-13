#ifndef GESTIONSERIE_H
#define GESTIONSERIE_H

#include "../mailbox/mailbox.h"
#include "../multithreadObjects/mutex.h"


struct ArgControleur {
	Mutex* mtxPauseRequest;
	Mutex* mtxSerieEnCours;
	Mailbox<Event>* eventBox;
};

void gestionSerie(void* params);

#endif // GESTIONSERIE_H
