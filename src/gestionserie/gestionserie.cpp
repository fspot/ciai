#include "gestionserie.h"
#include "../modeles/modeles.h"
#include <iostream>

#include <errno.h>

void ecriture_log_gestionSerie(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


void gestionserie_thread(void * params)
{
	ArgGestionSerie * args = (ArgGestionSerie*) params;
        ecriture_log_gestionSerie(args->gestionnaireLog,"Lancement de la tâche gestion series",EVENT);
	while(1)
	{
		sem_wait(args->finDeSerie);
 		ecriture_log_gestionSerie(args->gestionnaireLog,"Fin de lot detectée - gestionnaire series",EVENT);
		if (sem_trywait(args->mtxPauseRequest) != -1 && errno == EAGAIN )
		{
			args->eventBox->Push(Event(PAUSE),0);
 			ecriture_log_gestionSerie(args->gestionnaireLog,"Pause mise en place - gestionnaire series",EVENT);
		}

		args->eventBox->Push(Event(REPRISEPAUSE),0);
	}
}
