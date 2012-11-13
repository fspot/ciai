#include "gestionserie.h"
#include "../modeles/modeles.h"
#include <iostream>



void gestionserie_thread(void* params)
{
	ArgGestionSerie* args = (ArgGestionSerie*) params;
	while(1)
	{
		std::cout<<"I love cats"<<std::endl;
		args->mtxSerieEnCours->lock();
		if (args->mtxPauseRequest->trylock())
			args->eventBox->Push(Event(PAUSE),0);

		args->eventBox->Push(Event(REPRISEPAUSE),0);
	}
}

