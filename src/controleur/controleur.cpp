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
  clapet->trylock();
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
    ecriture_log_controleur(args->gestionnaireLog,"Le controleur à recu un nouvel évènement à traiter",EVENT);
    Message aMsg = {"", false};
    ListeCommandes lca;
    switch (nextEvent.event)
    {
	// Abscence d'un carton recue
    	case ABSCARTON:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: il y a abscence de carton, l'application est en attente.",ERROR);
    	  aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Panne de l'iprimante détéctée
    	case PANNEIMPRIM:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: il y a panne de l'imprimante, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Abscence palette detectée
    	case ABSPALETTE:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: il y a abscence de palette, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Taux d'erreur trop élevé
    	case TAUXERR:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: Le taux d'erreur est trop élevé, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// File d'attente pleine
    	case FILEATTPLEINE:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: La file d'attente est pleine, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;      

	// Arret d'urgence detecté
    	case ARTURG:
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: Un arret d'urgence a été enclenché, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Reprise suite à une erreur demandée
    	case REPRISEERREUR:
    	  ouverture_clapet(args->clapet);
    	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
    	  reprendre_tache(args->threads[STOCKERPALETTE]);
    	  reprendre_tache(args->threads[REMPLIRPALETTE]);
    	  reprendre_tache(args->threads[IMPRIMER]);
    	  reprendre_tache(args->threads[REMPLIRCARTON]);
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: Un ordre de reprise post érreur a été donné.",EVENT);
    	  break;

	// Une pause a été demandée
    	case PAUSE:
    	  sem_post(args->pauseSerieMutex);
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: Une demande une pause a été faite, elle sera traitée à la fin du prochain lot",EVENT);
    	  break;

	// Un lot/une serie est finie
    	case FINSERIE:
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: Une série est finie",EVENT);
    	  if (sem_trywait(args->pauseSerieMutex) != -1 )
	  {
    	    ecriture_log_controleur(args->gestionnaireLog,"Evenement: La pause programmée est effective",EVENT);
            fermeture_clapet(args->clapet);
	  }
    	  break;

	// Une erreur d'embalage est survenue
    	case ERREMBALAGES:
    	  fermeture_clapet(args->clapet);
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: Une erreur est survenue pendant un embalage, l'application est en attente.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Une erreur de commande est survenue
    	case ERRCOMMANDE:
    	  ecriture_log_controleur(args->gestionnaireLog,"Erreur: La commande n'as pa aboutie.",ERROR);
          aMsg.contenu = nextEvent.netstr();
    	  args->balMessages->Push(aMsg,1);
    	  break;

	// Une reprise après une pause
    	case REPRISEPAUSE:
    	  ouverture_clapet(args->clapet);
    	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
    	  reprendre_tache(args->threads[STOCKERPALETTE]);
    	  reprendre_tache(args->threads[REMPLIRPALETTE]);
    	  reprendre_tache(args->threads[IMPRIMER]);
    	  reprendre_tache(args->threads[REMPLIRCARTON]);
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: Une reprise a été demandé.",EVENT);
    	  break;

	// La dernière piece a érté envoyée
    	case FINLAST:
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: La dernière piece pour la dernière série est passée.",EVENT);
          fermeture_clapet(args->clapet);
          break;
          
	// La dernière palette a été stockée
    	case FIN:
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: la dernière palette a été stockée, terminaison.",EVENT);
          lca.fin=true;
          args->balCommandes->Push(lca,0);
          aMsg.fin = true;
          args->balMessages->Push(aMsg,1);
          aMsg.contenu = nextEvent.netstr();
    	  ecriture_log_controleur(args->gestionnaireLog,"Fin de la tâche controleur.",EVENT);
          pthread_exit(0);
          break;

	// Fin de l'application suite à une erreur
    	case FINERREUR:
    	  ecriture_log_controleur(args->gestionnaireLog,"Evenement: Un ordre de terminaison a été demandé suite à une erreur.",EVENT);
          // Déblocage des tâches :
          reprendre_tache(args->threads[DESTOCKERPALETTE]);
          reprendre_tache(args->threads[STOCKERPALETTE]);
          reprendre_tache(args->threads[REMPLIRPALETTE]);
          reprendre_tache(args->threads[IMPRIMER]);
          reprendre_tache(args->threads[REMPLIRCARTON]);
	  // Arret des taches 
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
          args->balMessages->Push(aMsg,1);
    	  ecriture_log_controleur(args->gestionnaireLog,"Fin de la tâche controleur.",EVENT);
          pthread_exit(0);
          break;

    	default:
        ecriture_log_controleur(args->gestionnaireLog,"Un événement non gérré a été envoyé au controleur",ERROR);
        break;
    }
  }
}
