#include <iostream>
#include "mailbox/mailbox.h"
#include "remplirPalette/remplirPalette.h"
#include "log/log.h"
#include "mutex/mutex.h"
 #include <iomanip>
using namespace std;


bool alwaysTrue()
{
	return true;
}

bool alwaysFalse()
{
	return false;
}

int main(int argc, char **argv) {
	
	//init params
	Mailbox<Carton> SbalPalette;
	Mutex mtxStandardOutput;
	Log SgestionnaireLog(mtxStandardOutput);
	Mailbox<Palette> SbalStockage;
	Mailbox<Message> SbalMessages;
	Mailbox<Event> SeventBox;
	pthread_cond_t Scw = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t Smxcw;
	pthread_mutex_init(&Smxcw,NULL);
	sem_t SafterErrorRestart;
	SharedMemoryLots SshMemLots;
	sem_t SdebutSyncro;
	sem_init(&SdebutSyncro, 0, 0);

	ListeLots listelosts;
	SshMemLots.content = &listelosts;
	SshMemLots.content->lots.reserve(3);

	SshMemLots.content->lots[0].cartons = 3;
	SshMemLots.content->lots[0].palettes = 3;
	SshMemLots.content->lots[1].cartons = 4;
	SshMemLots.content->lots[1].palettes = 4;
	SshMemLots.content->lots[2].cartons =5;
	SshMemLots.content->lots[2].palettes = 5;


	for (int i = 0 ; i<50 ; i++)	{
		Carton carton = {i,&SshMemLots.content->lots[0], 0};
		SbalPalette.Push(carton);
	}

	ArgsRemplirPalette args;
	args.balPalette = &SbalPalette;
	args.balStockage = &SbalStockage;
	args.gestionnaireLog = &SgestionnaireLog;
	args.balMessages = &SbalMessages;
	args.eventBox = &SeventBox;
	args.cw = &Scw;
	args.mxcw = &Smxcw;
	args.shMemLots = &SshMemLots;
	args.debutSyncro = &SdebutSyncro;
	args.capteurEmbalage = alwaysFalse; 	//jamais de probleme d'embalage
	args.capteurPalette = alwaysFalse;		//toujours une palette

	string strOk = " OK";
	string strErr = " ERREUR";

	pthread_t remplir_palette;
	pthread_create (&remplir_palette, NULL, (void *(*)(void *)) &remplirPalette_thread, (void *)&args);
	
	//sem_post(&SdebutSyncro);	//lancement de la tache

	cout << "Debut: test fonctionnement normal" << endl;

	sleep(2);
	
	cout << "cartons en entrée, attendu 0, effectif:" << SbalPalette.Size() << endl;
	cout << setw(70) << right << (SbalPalette.Size() == 0 ? strOk : strErr ) << endl;

	cout << "palettes en sortie, attendu 3, effectif:" << SbalStockage.Size() << endl;
	cout << setw(70) << right << (SbalStockage.Size() == 3 ? strOk : strErr ) << endl;
	
	cout << "ecriture dans bal Message? (destiné au client)" << endl;
	cout << setw(70) << right << (SbalMessages.Size() != 0 ? strOk : strErr ) << endl;

	cout << "Evenements (erreurs), attendu 0, effectif:" << SeventBox.Size() << endl;
	cout << setw(70) << right << (SeventBox.Size() == 0 ? strOk : strErr ) << endl;

	
	cout << "Fin: test fonctionnement normal" << endl << endl;


    return 0;
}
