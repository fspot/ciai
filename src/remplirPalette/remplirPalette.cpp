#include "remplirPalette.h"


void ecriture_log_remplirPalette(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

void remplirPalette_thread(void * argsUncasted)
{
    static unsigned int idpalette = 0;
    ArgsRemplirPalette * args = (ArgsRemplirPalette *) argsUncasted;
    ecriture_log_remplirPalette(args->gestionnaireLog,"Lancement de la tache remplir palette",EVENT);
    Palette palette;
    int countCarton = 0, countPalette = 0, countLot = 0;

    while (1)
    {
    	Carton carton = args->balPalette->Pull(); // appel bloquant
		ecriture_log_remplirPalette(args->gestionnaireLog,"Carton recu - remplir palette",EVENT);
    	if (carton.fin)
        {	
			ecriture_log_remplirPalette(args->gestionnaireLog,"Fin de la tache - remplir palette",EVENT);
			Palette p;
			p.fin=true;
    		args->balStockage->Push(p,0);
    		pthread_exit(0);
		}
		// Message réseau carton empaletté :
		Message msg = {carton.netstr_palette(), false};
		args->balMessages->Push(msg, 2);

    	// passage au carton suivant :
    	countCarton++;
    	if (countCarton == args->shMemLots->content->lots[countLot].cartons) { // next palette
	    	// on emballe la palette qu'on vient de finir :
    		if (stubErrEmbalagePalette()) { // si erreur emballage :
    			args->eventBox->Push(Event(ABSPALETTE),0);
    			pthread_mutex_lock(args->mxcw);
    			pthread_cond_wait(args->cw,args->mxcw);
    			pthread_mutex_unlock(args->mxcw);
    		}

    		// On donne un id et un id_lot à la palette finie :
    		palette.id = idpalette++;
    		args->shMemLots->mutex.lock();
    		palette.lot = &args->shMemLots->content->lots[countLot];
    		args->shMemLots->mutex.unlock();

    		// on push la palette
    		args->balStockage->Push(palette,0);
    		
    		// Message réseau palette finie :
    		Message msg = {palette.netstr(), false};
    		args->balMessages->Push(msg, 2);

    		countCarton = 0;
	    	countPalette++;
	    	if (countPalette == args->shMemLots->content->lots[countLot].palettes) { // next lot
	    		countPalette = 0;
	    		countLot++;
	    		// test de fin de production :
	    		if (countLot == args->shMemLots->content->lots.size())
	    			break;
	    	}

	    	// la palette est elle bien présente ?
	    	if (! stubPresencePalette()) { // palette absente !
	    		args->eventBox->Push(Event(ERREMBALAGES),0);
	    		pthread_mutex_lock(args->mxcw);
	    		pthread_cond_wait(args->cw,args->mxcw);
	    		pthread_mutex_unlock(args->mxcw);
	    	}
	    }
    }
    pthread_exit(0);
}

/* // debut ancienne version
sem_wait(args->debutSyncro);
while (1)
{
	
	args->shMemLots->mutex.lock();
	int cartonsMax = args->shMemLots->content->lots[lotnb].cartons;
	int palettesMax = args->shMemLots->content->lots[lotnb].palettes;
	args->shMemLots->mutex.unlock();
	int countPalettes = 0;

	while (countPalettes < palettesMax)
	{
		if (stubPresencePalette())
		{
			Palette palette;
			palette.id = idpalette++;
			int countCartons = 0;
			while (countCartons < cartonsMax)
			{
				Carton carton = args->balImprimante->Pull();	//cartons perdus (pas nécessaire de les garder d'après le client)
				if (carton.fin)
					pthread_exit(0);
				args->shMemLots->mutex.lock();
				palette.lot = &args->shMemLots->content->lots[lotnb];
				args->shMemLots->mutex.unlock();
				countCartons++;
			}
			if (stubErrEmbalagePalette())
			{
				//traitement de l'erreur: erreur d'embalage d'une palette
				args->eventBox->Push(Event(ABSPALETTE),0);
				pthread_mutex_lock(args->mxcw);
				pthread_cond_wait(args->cw,args->mxcw);
			
				pthread_mutex_unlock(args->mxcw);
			}
			args->balPalette->Push(palette,0);
			
		}
		else
		{
			//traitement de l'erreur: absence palette
			args->eventBox->Push(Event(ERREMBALAGES),0);
			pthread_mutex_lock(args->mxcw);
			pthread_cond_wait(args->cw,args->mxcw);
			pthread_mutex_unlock(args->mxcw);
		}
	}
	lotnb++;
	if (lotnb == args->shMemLots->content->tot)
		pthread_exit(0);
}
// end ancienne version */

