#include "gestionserie.h"
#include "../modeles/modeles.h"




void gestionSerie(void* params)
{
	ArgControleur* args = (ArgControleur*) params;
	while(1)
	{
		args->mtxSerieEnCours->lock();
		if (args->mtxPauseRequest->trylock())
			args->eventBox->Push(Event(PAUSE));

		args->eventBox->Push(Event(REPRISEPAUSE));
	}
}

