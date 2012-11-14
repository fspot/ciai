#include "remplirPalette.h"

void remplirPalette(void * argsUncasted)
{
	static unsigned int idpalette = 0;
    remplirPaletteArgs args = (remplirPaletteArgs) *argsUncasted;
    unsigned int lotnb = 0;
    while (1)
    {

		args.shMemLots->mutex.lock();
		int cartonsMax = args.shMemLots->content->lots[lotnb].cartons;
		int palettesMax = args.shMemLots->content->lots[lotnb].palettes;
		args.shMemLots->mutex.unlock();
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
					Carton carton = args.balImprimante->Pull();	//cartons perdus (pas nécessaire de les garder d'après le client)
					if (carton.fin)
						pthread_exit();
					args.shMemLots->mutex.lock();
					palette.lot = args.shMemLots->content->lots[lotnb];
					args.shMemLots->mutex.unlock();
					countCartons++;
				}
				if (stubErrEmbalagePalette())
				{
					//traitement de l'erreur: erreur d'embalage d'une palette
					args.eventBox.Push(Event(ABSPALETTE));
					sem_wait(args.afterErrorRestart);
				}
				args.balPalette.Push(palette);
			}
			else
			{
				//traitement de l'erreur: absence palette
				args.eventBox.Push(Event(ERREMBALAGES));
				sem_wait(args.afterErrorRestart);
			}
		}
		lotnb++;
		if (lotnb == args.shMemLots->content->tot)
			pthread_exit();
            

    }
}