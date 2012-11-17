#include "gestionserie.h"
#include "../modeles/modeles.h"
#include <iostream>

#include <errno.h>

void gestionserie_thread(void * params)
{
	ArgGestionSerie * args = (ArgGestionSerie*) params;
	while(1)
	{
		sem_wait(args->finDeSerie);
		if (sem_trywait(args->mtxPauseRequest) != -1 && errno == EAGAIN )
			args->eventBox->Push(Event(PAUSE),0);

		args->eventBox->Push(Event(REPRISEPAUSE),0);
	}
}
