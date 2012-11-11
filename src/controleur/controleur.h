#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
using namespace std;


struct ArgControleur
{
  Mailbox<Event> * eventBox;
  Mailbox<Message> * msgBox;
  
};

void fermeture_clapet()
{
  // a ecrire
}

void ecriture_log_controleur()
{
  // a ecrire
}

void suspendre_tache(Task aTask)
{
  // a ecrire
}


void reprendre_tache(Task aTask)
{
  // a ecrire
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
	  suspendre_tache(REMPLIRCARTON);
	  ecriture_log_controleur();
	  aMsg.contenu ="Panne imprimante";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ABSPALETTE:
	  fermeture_clapet();
	  suspendre_tache(REMPLIRCARTON);
	  suspendre_tache(IMPRIMER);
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
	  suspendre_tache(REMPLIRCARTON);
	  suspendre_tache(IMPRIMER);
	  ecriture_log_controleur();
	  aMsg.contenu ="File d'attente pleine";
	  args->msgBox->Push(aMsg,0);
	  break;      


	case ARTURG:
	  suspendre_tache(REMPLIRCARTON);
	  suspendre_tache(IMPRIMER);
	  suspendre_tache(REMPLIRPALETTE);
	  suspendre_tache(STOCKERPALETTE);
	  suspendre_tache(DESTOCKERPALETTE);
	  ecriture_log_controleur();
	  aMsg.contenu ="Arret d'urgence";
	  args->msgBox->Push(aMsg,0);
	  break;


	case REPRISEERREUR:
	  reprendre_tache(DESTOCKERPALETTE);
	  reprendre_tache(STOCKERPALETTE);
	  reprendre_tache(REMPLIRPALETTE);
	  reprendre_tache(IMPRIMER);
	  reprendre_tache(REMPLIRCARTON);
	  ecriture_log_controleur();
	  aMsg.contenu ="Reprise pause";
	  args->msgBox->Push(aMsg,0);
	  break;

	case PAUSE:
	  fermeture_clapet();
	  suspendre_tache(REMPLIRCARTON);
	  suspendre_tache(IMPRIMER);
	  suspendre_tache(REMPLIRPALETTE);
	  suspendre_tache(STOCKERPALETTE);
	  suspendre_tache(DESTOCKERPALETTE);
	  ecriture_log_controleur();
	  aMsg.contenu ="Pause";
	  args->msgBox->Push(aMsg,0);
	  break;

	case ERREMBALAGES:
	  fermeture_clapet();
	  suspendre_tache(REMPLIRCARTON);
	  suspendre_tache(IMPRIMER);
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
	  reprendre_tache(STOCKERPALETTE);
	  reprendre_tache(REMPLIRPALETTE);
	  reprendre_tache(IMPRIMER);
	  reprendre_tache(REMPLIRCARTON);

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
