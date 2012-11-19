/*************************************************************************
                           controleur  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche controleur

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <map>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
//------------------------------------------------------ Include personnel
#include "controleur.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées


// Méthode de fermeture de clapet
void fermeture_clapet(Mutex * clapet)
{
  clapet->lock();
}


// Méthode d'ouverture de clapet
void ouverture_clapet(Mutex * clapet)
{
   clapet->unlock();
}

// Méthode d'écriture dans le log
void ecriture_log_controleur(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

//Méthode de reprise de tache
void reprendre_tache(InfoThread aThread)
{
  pthread_cond_signal(aThread.cw);
}


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

//Fonction du thread controleur
int controleur_thread(void * argsUnconverted)
{
  // Conversion des paramètre
  ArgControleur  * args= (ArgControleur *)argsUnconverted;

  ecriture_log_controleur(args->gestionnaireLog,"Lancement de la tâche controleur",EVENT);
  while(1)
  {
    // Attente de récupération d'événement
    Event nextEvent = args->balEvenements->Pull();
    ecriture_log_controleur(args->gestionnaireLog,"Nouvel evenement a traiter - controleur",EVENT);
    Message aMsg = {"", false};
    ListeCommandes lca;
    switch (nextEvent.event)
    {
    	case ABSCARTON:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Abscence carton",ERROR);
    	  aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case PANNEIMPRIM:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Panne imprimante",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case ABSPALETTE:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Abscence palette",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case TAUXERR:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Taux erreur",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case FILEATTPLEINE:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"File d'attente pleine",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;      

    	case ARTURG:
    	  ecriture_log_controleur(args->gestionnaireLog,"Arret d'urgence",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case REPRISEERREUR:
    	  ouverture_clapet(args->clapet);
    	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
    	  reprendre_tache(args->threads[STOCKERPALETTE]);
    	  reprendre_tache(args->threads[REMPLIRPALETTE]);
    	  reprendre_tache(args->threads[IMPRIMER]);
    	  reprendre_tache(args->threads[REMPLIRCARTON]);
    	  ecriture_log_controleur(args->gestionnaireLog,"Reprise pause",ERROR);
    	  break;

    	case PAUSE:
    	  sem_post(args->pauseSerieMutex);
    	  ecriture_log_controleur(args->gestionnaireLog,"Demande de pause traitée",EVENT);
    	  break;
    	case FINSERIE:
    	  if (sem_trywait(args->pauseSerieMutex) != -1 )
	  {
    	    ecriture_log_controleur(args->gestionnaireLog,"En pause",EVENT);
            fermeture_clapet(args->clapet);
	  }
    	  break;

    	case ERREMBALAGES:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog, "Erreur embalage",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case ERRCOMMANDE:
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur commande",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,0);
    	  break;

    	case REPRISEPAUSE:
    	  ouverture_clapet(args->clapet);
    	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
    	  reprendre_tache(args->threads[STOCKERPALETTE]);
    	  reprendre_tache(args->threads[REMPLIRPALETTE]);
    	  reprendre_tache(args->threads[IMPRIMER]);
    	  reprendre_tache(args->threads[REMPLIRCARTON]);
    	  ecriture_log_controleur(args->gestionnaireLog,"Reprise pause",EVENT);
    	  break;
    	case FINLAST:
          fermeture_clapet(args->clapet);
          break;
    	case FIN:
          lca.fin=true;
          args->balCommandes->Push(lca,0);
          aMsg.fin = true;
          args->balMessages->Push(aMsg,0);
          aMsg.contenu = nextEvent.netstr();
	  //Fin d'application à envoyer
    	  //args->balMessages->Push(aMsg,0);
          pthread_exit(0);
          break;
    	case FINERREUR:
          ecriture_log_controleur(args->gestionnaireLog,"Terminaison de l'application apres erreur - controleur",EVENT);
	  // Arret des taches suivantes
          Carton c2;
          c2.fin=true;
          args->balPalette->Push(c2,0);
          Palette p;
          p.fin=true;
          args->balStockage->Push(p,0);
          Piece pe;
          pe.fin=true;
          args->balPiece->Push(pe,0);
          Carton c1;
          c1.fin=true;
          args->balImprimante->Push(c1,0);
          lca.fin=true;
          args->balCommandes->Push(lca,0);
          aMsg.fin = true;
          args->balMessages->Push(aMsg,0);
          ecriture_log_controleur(args->gestionnaireLog,"Fin de la tâche controleur",EVENT);
          pthread_exit(0);
          break;

    	default:
        ecriture_log_controleur(args->gestionnaireLog,"Erreur non gérée",ERROR);
        break;
    }
  }
}
