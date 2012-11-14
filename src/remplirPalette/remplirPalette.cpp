#include "remplirPalette.h"

void remplirPalette(void * argsUncasted)
{
	static unsigned int idpalette = 0;
    remplirPaletteArgs args = (remplirPaletteArgs) *argsUncasted;
    unsigned int lotnb = 0;
    while (1)
    {
        if (stubPresencePalette())
        {
            args.shMemLots->mutex.lock();
            int cartonsNb = args.shMemLots->content->lots[lotnb].cartons;
			args.shMemLots->mutex.unlock();
			int count = 0;
			Palette palette;
			palette.id = idpalette++;
            while (count < cartonsNb)
            {
				Carton carton = args.balImprimante->Pull();	//cartons perdus (pas nécessaire de les garder d'après le client)
				if (carton.fin)
					pthread_exit();
				args.shMemLots->mutex.lock();
				palette.lot = args.shMemLots->content->lots[lotnb];
				args.shMemLots->mutex.unlock();
                count++;
            }
            if (stubErrEmbalagePalette())
			{
				//traitement de l'erreur: erreur d'embalage d'une palette
				args.eventBox.Push(Event(ABSPALETTE));
				sem_wait(args.afterErrorRestart);
			}
            args.balPalette.Push(palette);
			
			lotnb++;
			if (lotnb == args.shMemLots->content->tot)
				pthread_exit();
            
        }
        else
        {
            //traitement de l'erreur: absence palette
			args.eventBox.Push(Event(ERREMBALAGES));
			sem_wait(args.afterErrorRestart);
        }
    }
}