#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include <pthread.h>
#include <signal.h>
#include "../log/log.h" 
using namespace std;

#include <map>

struct InfoThread
{
  pthread_t id;
  pthread_cond_t * cw;
  pthread_mutex_t * mx;
};
struct ArgControleur
{
  Mailbox<Event> * eventBox;
  Mailbox<Message> * msgBox;
  Log * gestionnaireLog;
  map<Task,InfoThread> threads;
};

void fermeture_clapet()
{
  // a ecrire
}

void ecriture_log_controleur(Log * unGestionnaire, std::string msg,logType unType)
{
  #ifdef DEBUG
  unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

void suspendre_tache(InfoThread aThread)
{
  pthread_kill(aThread.id,SIGUSR2);
}


void reprendre_tache(InfoThread aThread)
{
  pthread_cond_signal(aThread.cw);
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
	  ecriture_log_controleur(args->gestionnaireLog,"Abscence carton",ERROR);
	  aMsg.contenu ="Abscence carton";
	  args->msgBox->Push(aMsg,0);
	  break;

	case PANNEIMPRIM:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  ecriture_log_controleur(args->gestionnaireLog,"Panne imprimante",ERROR);

	  aMsg.contenu ="Panne imprimante";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ABSPALETTE:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur(args->gestionnaireLog,"Abscence palette",ERROR);
	  aMsg.contenu ="Abscence palette";
	  args->msgBox->Push(aMsg,0);
	  break;

	case TAUXERR:
	  fermeture_clapet();
	  ecriture_log_controleur(args->gestionnaireLog,"Taux erreur",ERROR);
	  aMsg.contenu ="Taux erreur";
	  args->msgBox->Push(aMsg,0);
	  break;

	case FILEATTPLEINE:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur(args->gestionnaireLog,"File d'attente pleine",ERROR);
	  aMsg.contenu ="File d'attente pleine";
	  args->msgBox->Push(aMsg,0);
	  break;      


	case ARTURG:
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  suspendre_tache(args->threads[REMPLIRPALETTE]);
	  suspendre_tache(args->threads[STOCKERPALETTE]);
	  suspendre_tache(args->threads[DESTOCKERPALETTE]);
	  ecriture_log_controleur(args->gestionnaireLog,"Arret d'urgence",ERROR);
	  aMsg.contenu ="Arret d'urgence";
	  args->msgBox->Push(aMsg,0);
	  break;


	case REPRISEERREUR:
	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
	  reprendre_tache(args->threads[STOCKERPALETTE]);
	  reprendre_tache(args->threads[REMPLIRPALETTE]);
	  reprendre_tache(args->threads[IMPRIMER]);
	  reprendre_tache(args->threads[REMPLIRCARTON]);
	  ecriture_log_controleur(args->gestionnaireLog,"Reprise pause",ERROR);
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
	  ecriture_log_controleur(args->gestionnaireLog,"Pause fin de serie",EVENT);
	  aMsg.contenu ="Pause fin de serie";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ERREMBALAGES:
	  fermeture_clapet();
	  suspendre_tache(args->threads[REMPLIRCARTON]);
	  suspendre_tache(args->threads[IMPRIMER]);
	  ecriture_log_controleur(args->gestionnaireLog, "Erreur embalage",ERROR);
	  aMsg.contenu ="Erreur embalage";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ERRCOMMANDE:
	  ecriture_log_controleur(args->gestionnaireLog,"Erreur commande",ERROR);
	  aMsg.contenu ="Erreur commande";
	  args->msgBox->Push(aMsg,0);
	  break;

	case REPRISEPAUSE:
	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
	  reprendre_tache(args->threads[STOCKERPALETTE]);
	  reprendre_tache(args->threads[REMPLIRPALETTE]);
	  reprendre_tache(args->threads[IMPRIMER]);
	  reprendre_tache(args->threads[REMPLIRCARTON]);

	  ecriture_log_controleur(args->gestionnaireLog,"Reprise pause",EVENT);
	  aMsg.contenu ="Reprise pause";
	  args->msgBox->Push(aMsg,0);
	  break;
	default:
	    std::cout<<"Message d'erreur non supporté"<<std::endl;
	break;
	}
    }

}
