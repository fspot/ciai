#include <iostream>
#include "src/mailbox/mailbox.h"
#include "src/remplirPalette/remplirPalette.h"

int main(int argc, char **argv) {

	Mailbox<Carton> SbalImprimante;
	Mailbox<Palette> SbalPalette;
	Mailbox<Event> SeventBox;
	sem_t SafterErrorRestart;
	SharedMemoryLots SshMemLots;
	
	SshMemLots.content->lots[0].cartons = 3;
	SshMemLots.content->lots[0].palettes = 3;
	SshMemLots.content->lots[0].cartons = 3;
	SshMemLots.content->lots[0]. = 3;
	
	for (int i = 0 ; i<10 ; i++)
	{
		Carton carton;
		SbalImprimante.Push(carton);
	}
	
	ArgGestionSerie args;
	args.balImprimante = &SbalImprimante;
	args.balPalette = &SbalPalette;
	args.eventBox = &SeventBox;
	args.afterErrorRestart = &SafterErrorRestart;
	args.shMemLots = &SshMemLots;
	

	
	
	
    return 0;
}
