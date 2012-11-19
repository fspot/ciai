#include <iostream>
#include "src/mailbox/mailbox.h"
#include "src/remplirPalette/remplirPalette.h"
#include "src/log//log.h"

int main(int argc, char **argv) {

	Mailbox<Carton> SbalImprimante;
	Log SgestionnaireLog;
	Mailbox<Palette> SbalPalette;
	Mailbox<Event> SeventBox;
	pthread_cond_t Scw = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t Smxcw;
	sem_t SafterErrorRestart;
	SharedMemoryLots SshMemLots;
	sem_t SdebutSyncro;
	sem_init(&SdebutSyncro, 0, 0);
	
	SshMemLots.content->lots[0].cartons = 3;
	SshMemLots.content->lots[0].palettes = 3;
	SshMemLots.content->lots[1].cartons = 4;
	SshMemLots.content->lots[1].palettes = 4;
	SshMemLots.content->lots[2].cartons =5;
	SshMemLots.content->lots[2].palettes = 5;
	
	
	for (int i = 0 ; i<10 ; i++)	{
		Carton carton;
		SbalImprimante.Push(carton);
	}
	
	ArgGestionSerie args;
	args.balImprimante = &SbalImprimante;
	args.gestionnaireLog = &SgestionnaireLog;
	args.balPalette = &SbalPalette;
	args.eventBox = &SeventBox;
	args.cw = &Scw;
	args.mxcw = &Smxcw;
	args.shMemLots = &SshMemLots;
	args.debutSyncro = &SdebutSyncro;
	
	pthread_t remplir_palette;
	pthread_create (&remplir_palette, NULL, (void *(*)(void *)) &remplirPalette_thread, args);

	

	
	
	
    return 0;
}
