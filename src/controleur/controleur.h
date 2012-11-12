#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include <pthread.h>
#include <signal.h>
using namespace std;

#include <map>

struct ArgControleur
{
  Mailbox<Event> * eventBox;
  Mailbox<Message> * msgBox;
  map<Task,pthread_t> threads;
};

void fermeture_clapet()
{
  // a ecrire
}

void ecriture_log_controleur()
{
  // a ecrire
}

void suspendre_tache(pthread_t aThread)
{
  pthread_kill(aThread,SIGUSR2);
}


void reprendre_tache(pthread_t aThread)
{
  pthread_kill(aThread,SIGUSR1);
}

int controleur_thread(void * argsUnconverted)
{
  ArgControleur  * args= (ArgControleur *)argsUnconverted;
  std::cout<<"Tâche controleur lancée"<<std::endl;
  while(1)
    {
      std::cout<<"Mise en attente  -- thread controleur"<<std::endl;
      Event nextEvent = args->eventBox->Pull();
      std::cout<<"Un message recu  -- thread controleur"<<std::endl;
      Message aMsg;
      switch (nextEvent.event)
	{

	case ABSCARTON:
	  fermeture_clapet();
	  ecriture_log_controleur();

	  aMsg.contenu ="Abscence carton";
	  args->msgBox->Push(aMsg,0);
	  break;

	case PANNEIMPRIM:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  ecriture_log_controleur();
	  aMsg.contenu ="Panne imprimante";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ABSPALETTE:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur();
	  aMsg.contenu ="Abscence palette";
	  args->msgBox->Push(aMsg,0);
	  break;

	case TAUXERR:
	  fermeture_clapet();
	  ecriture_log_controleur();
	  aMsg.contenu ="Taux erreur";
	  args->msgBox->Push(aMsg,0);
	  break;

	case FILEATTPLEINE:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur();
	  aMsg.contenu ="File d'attente pleine";
	  args->msgBox->Push(aMsg,0);
	  break;      


	case ARTURG:
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  suspendre_tache(args->threads[REMPLIRPALETTE]);
	  suspendre_tache(args->threads[STOCKERPALETTE]);
	  suspendre_tache(args->threads[DESTOCKERPALETTE]);
	  ecriture_log_controleur();
	  aMsg.contenu ="Arret d'urgence";
	  args->msgBox->Push(aMsg,0);
	  break;


	case REPRISEERREUR:
	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
	  reprendre_tache(args->threads[STOCKERPALETTE]);
	  reprendre_tache(args->threads[REMPLIRPALETTE]);
	  reprendre_tache(args->threads[IMPRIMER]);
	  reprendre_tache(args->threads[REMPLIRCARTON]);
	  ecriture_log_controleur();
	  aMsg.contenu ="Reprise pause";
	  args->msgBox->Push(aMsg,0);
	  break;

	case PAUSE:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  suspendre_tache(args->threads[REMPLIRPALETTE]);
	  suspendre_tache(args->threads[STOCKERPALETTE]);
	  suspendre_tache(args->threads[DESTOCKERPALETTE]);
	  ecriture_log_controleur();
	  aMsg.contenu ="Pause";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ERREMBALAGES:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur();

	  aMsg.contenu ="Erreur embalage";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ERRCOMMANDE:
	  ecriture_log_controleur();
	  aMsg.contenu ="Erreur commande";
	  args->msgBox->Push(aMsg,0);
	  break;

	case REPRISEPAUSE:
	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
	  reprendre_tache(args->threads[STOCKERPALETTE]);
	  reprendre_tache(args->threads[REMPLIRPALETTE]);
	  reprendre_tache(args->threads[IMPRIMER]);
	  reprendre_tache(args->threads[REMPLIRCARTON]);

	  ecriture_log_controleur();
	  aMsg.contenu ="Reprise pause";
	  args->msgBox->Push(aMsg,0);
	  break;
	default:
	    std::cout<<"wtf?"<<std::endl;
	break;
	}
    }

}
