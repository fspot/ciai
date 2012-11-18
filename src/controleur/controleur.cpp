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
//------------------------------------------------------ Include personnel
#include "controleur.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

void fermeture_clapet(Mutex * clapet)
{
   cout<<"Clapet fermé"<<endl;
   clapet->lock();
}

void ouverture_clapet(Mutex * clapet)
{
   cout<<"Clapet ouvert"<<endl;
   clapet->unlock();

}


void ecriture_log_controleur(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


void reprendre_tache(InfoThread aThread)
{
  pthread_cond_signal(aThread.cw);
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


int controleur_thread(void * argsUnconverted)
{
  ArgControleur  * args= (ArgControleur *)argsUnconverted;
  ecriture_log_controleur(args->gestionnaireLog,"Lancement de la tâche controleur",EVENT);
  while(1)
    {
      Event nextEvent = args->balEvenements->Pull();
      ecriture_log_controleur(args->gestionnaireLog,"Nouvel evenement a traiter - controleur",EVENT);
      Message aMsg;
      ListeCommandes lca;
      switch (nextEvent.event)
	{

	case ABSCARTON:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"Abscence carton",ERROR);
	  aMsg.contenu ="Abscence carton - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case PANNEIMPRIM:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"Panne imprimante",ERROR);
	  aMsg.contenu ="Panne imprimante - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case ABSPALETTE:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"Abscence palette",ERROR);
	  aMsg.contenu ="Abscence palette - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case TAUXERR:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"Taux erreur",ERROR);
	  aMsg.contenu ="Taux erreur - Tache controleur";
	  args->balMessages->Push(aMsg,0);

	  break;

	case FILEATTPLEINE:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"File d'attente pleine",ERROR);
	  aMsg.contenu ="File d'attente pleine - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;      


	case ARTURG:
	  ecriture_log_controleur(args->gestionnaireLog,"Arret d'urgence",ERROR);
	  aMsg.contenu ="Arret d'urgence - Tache controleur";
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
	  aMsg.contenu ="Reprise pause - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case PAUSE:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog,"Pause fin de serie",EVENT);
	  aMsg.contenu ="Pause fin de serie - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case ERREMBALAGES:
	  fermeture_clapet(args->clapet);
	  ecriture_log_controleur(args->gestionnaireLog, "Erreur embalage",ERROR);
	  aMsg.contenu ="Erreur embalage - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case ERRCOMMANDE:
	  ecriture_log_controleur(args->gestionnaireLog,"Erreur commande",ERROR);
	  aMsg.contenu ="Erreur commande - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;

	case REPRISEPAUSE:
	  reprendre_tache(args->threads[DESTOCKERPALETTE]);
	  reprendre_tache(args->threads[STOCKERPALETTE]);
	  reprendre_tache(args->threads[REMPLIRPALETTE]);
	  reprendre_tache(args->threads[IMPRIMER]);
	  reprendre_tache(args->threads[REMPLIRCARTON]);
	  ecriture_log_controleur(args->gestionnaireLog,"Reprise pause",EVENT);
	  aMsg.contenu ="Reprise pause - Tache controleur";
	  args->balMessages->Push(aMsg,0);
	  break;
	case FIN:
	  fermeture_clapet(args->clapet);
  	  lca.fin=true;
	  args->balCommandes->Push(lca,0);
	  pthread_exit(0);
          break;
	case FINERREUR:
           ecriture_log_controleur(args->gestionnaireLog,"Terminaison de l'application apres erreur - controleur",EVENT);
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
           ecriture_log_controleur(args->gestionnaireLog,"Fin de la tâche controleur",EVENT);
	   pthread_exit(0);
	  break;
	default:
	    ecriture_log_controleur(args->gestionnaireLog,"Erreur non gérée",ERROR);
	break;
	}

    }

}
