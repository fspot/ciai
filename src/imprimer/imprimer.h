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
  SharedMemoryLots * Lots;
};

int imprimer_thread(void * argsUnconverted)
{
  ArgImprimer * args= (ArgImprimer *)argsUnconverted;
  cout<<"Tâche imprimer lancée"<<endl;
  
  bool panneImprimante = false;
  Carton cartonImpression;
  int nbCartonFile = 0;

  pthread_cond_t varCond;
  pthread_mutex_t mutex;
  for (;;) 
    {
      cartonImpression = args->balImprimante->Pull();
      // Verification panne imprimante
      // panneImprimante = 

      if (!panneImprimante) {

	if (nbCartonFile < 10) {

	  args->balPalette->Push(cartonImpression,0);
	  nbCartonFile++; // décrémentation à faire du coté de la tache remplir_palette

	}
	else {

	  // Depot message erreur dans la bal balEvenenements
	  Event msgErreurFileAttente(FILEATTPLEINE);
	  args->eventBox->Push( msgErreurFileAttente,0 );

	  // Mise en attente d'un signal envoye par le controleur pour reprendre
	  pthread_mutex_lock( args->mutex );
	  pthread_cond_wait( args->varCond, args->mutex );
	  pthread_mutex_unlock( args->mutex );

	}

      }
      else {

	// Depot message erreur dans la bal balEvenenements
	Event msgErreurImp(PANNEIMPRIM);
	args->eventBox->Push( msgErreurImp ,0);

	// Mise en attente d'un signal envoye par le controleur pour reprendre
	pthread_mutex_lock( args->mutex );
	pthread_cond_wait( args->varCond, args->mutex );
	pthread_mutex_unlock( args->mutex );
      }
    }
}
