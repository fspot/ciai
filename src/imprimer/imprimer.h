#include <pthread.h>

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"

using namespace std;

struct ArgImprimer
{
  Mailbox<Event> * eventBox;
  Mailbox<Carton> * balImprimante;
  Mailbox<Carton> * balPalette;
  pthread_cond_t * varCond;
  pthread_mutex_t * mutex;
  SharedMemoryLots * Lots; // vérifier si fin du lot pour tuer tache, à faire
  sem_t * debutSyncro;
  bool * panneImprim;
  Mutex * mutexPanne;
};

int imprimer_thread(void * argsUnconverted)
{
	ArgImprimer * args= (ArgImprimer *)argsUnconverted;
	cout<<"Tâche imprimer lancée"<<endl;
	
	bool panneImprimante = false;
	Carton cartonImpression;
	
	pthread_cond_t varCond;
	pthread_mutex_t mutex;

	for (;;) {

		cartonImpression = args->balImprimante->Pull();
		if(cartonImpression.fin)
			pthread_exit(0);
		cout<<"\nCarton récupéré "<< cartonImpression.id<<endl;

		// Verification panne imprimante, panneImprim a simuler
		// mutexPanne->lock();
		// panneImprimante = args->panneImprim;
		// mutexPanne->unlock();

		if (!panneImprimante) {

			if (args->balPalette->Size() < 10) {

				// Depot Carton dans la bal balPalette
				args->balPalette->Push(cartonImpression,0);

			}
			else {

				// Depot message erreur dans la bal enventbox
				cout<<"File Pleine après impression"<<endl;
				Event msgErreurFileAttente(FILEATTPLEINE);
				args->eventBox->Push( msgErreurFileAttente,0 );
				
				// Mise en attente d'un signal envoye par le controleur pour reprendre
				pthread_mutex_lock( args->mutex );
				pthread_cond_wait( args->varCond, args->mutex );
				pthread_mutex_unlock( args->mutex );

			}

		}
		else {

			// Depot message erreur dans la bal eventbox
			Event msgErreurImp(PANNEIMPRIM);
			args->eventBox->Push( msgErreurImp ,0);
		
			// Mise en attente d'un signal envoye par le controleur pour reprendre
			pthread_mutex_lock( args->mutex );
			pthread_cond_wait( args->varCond, args->mutex );
			pthread_mutex_unlock( args->mutex );
		}
	}
}
