#include "remplirPalette.h"




void remplirPalette_thread(void * argsUncasted)
{
    static unsigned int idpalette = 0;
    ArgsRemplirPalette * args = (ArgsRemplirPalette *) argsUncasted;
    unsigned int lotnb = 0;
    while (1)
    {
		sem_wait(args->debutSyncro);
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
}
