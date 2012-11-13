#include "gestionserie.h"
#include "../modeles/modeles.h"




void gestionSerie(void* params)
{
	ArgControleur* args = (ArgControleur*) params;
	while(1)
	{
		args->mtxSerieEnCours->lock();
		if (args->mtxPauseRequest->getMutex())
			args->eventBox->Push(Event(PAUSE));

		args->mtxPauseRequest->lock();
		args->mtxPauseRequest->unlock();
		args->eventBox->Push(Event(REPRISEPAUSE));
	}
}

