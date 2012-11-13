#include "gestionserie.h"
#include "../modeles/modeles.h"
#include <iostream>



void gestionserie_thread(void* params)
{
	ArgGestionSerie args = (ArgGestionSerie) *params;
	while(1)
	{
		std::cout<<"I love cats"<<std::endl;
		sem_wait(args->finDeSerie);
		if (sem_trywait(args->mtxPauseRequest) != EAGAIN)
			args->eventBox->Push(Event(PAUSE),0);

		args->eventBox->Push(Event(REPRISEPAUSE),0);
	}
}
