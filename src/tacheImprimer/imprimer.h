#include <pthread.h>

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"


using namespace std;

struct ArgsImprimer
{
  Mailbox<Event> * eventBox;
  Mailbox<Carton> * balImprimante;
  Mailbox<Carton> * balPalette;
  pthread_cond_t * varCond;
  pthread_mutex_t * mutex;
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

  for (;;) {

    // Recuperation du carton a imprimer
    cartonImpression = args->balImprimante->pull();

    // Verification panne imprimante
    // panneImprimante = 

    if (!panneImprimante) {

      if (nbCartonFile < 10) {

        args->balPalette->push(cartonImpression);
        nbCartonFile++; // décrémentation à faire du coté de la tache remplir_palette

      }
      else {

        // Depot message erreur dans la bal balEvenenements
        Event msgErreurFileAttente;
        msgErreurFileAttente.event = FILEATTPLEINE;
        args->eventbox->push( msgErreurFileAttente );

        // Mise en attente d'un signal envoye par le controleur pour reprendre
        pthread_mutex_lock( args->mutex );
        pthread_cond_wait( args->varCond, atgs->mutex );
        pthread_mutex_unlock( args->mutex );

      }

    }
    else {

      // Depot message erreur dans la bal balEvenenements
      Event msgErreurImp;
      msgErreurImp.event = PANNEIMPRIM;
      args->eventbox->push( msgErreurImp );

      // Mise en attente d'un signal envoye par le controleur pour reprendre
      pthread_mutex_lock( args->mutex );
      pthread_cond_wait( args->varCond, args->mutex );
      pthread_mutex_unlock( args->mutex );
  }
}