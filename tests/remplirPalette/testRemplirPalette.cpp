#include <iostream>
#include "mailbox/mailbox.h"
#include "remplirPalette/remplirPalette.h"
#include "log/log.h"
using namespace std;

int main(int argc, char **argv) {
	cout<<"toto!"<<endl;
	Mailbox<Carton> SbalPalette;
	Mutex mtxStandardOutput;
	Log SgestionnaireLog(mtxStandardOutput);
	Mailbox<Palette> SbalStockage;
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
		SbalPalette.Push(carton);
	}

	ArgsRemplirPalette args;
	args.balPalette = &SbalPalette;
	args.balStockage = &SbalStockage;
	args.gestionnaireLog = &SgestionnaireLog;
	args.eventBox = &SeventBox;
	args.cw = &Scw;
	args.mxcw = &Smxcw;
	args.shMemLots = &SshMemLots;
	args.debutSyncro = &SdebutSyncro;

	pthread_t remplir_palette;
	pthread_create (&remplir_palette, NULL, (void *(*)(void *)) &remplirPalette_thread, (void *)&args);






    return 0;
}
