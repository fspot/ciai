/*************************************************************************
                           controleur  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche controleur

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

//------------------------------------------------------ Include personnel
#include "imprimer.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

// Méthode d'écriture dans le log
void ecriture_log_imprimer(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
#ifdef DEBUG
  unGestionnaire->Write(msg,unType,true);
#else
  unGestionnaire->Write(msg,unType,false);
#endif 
}


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int imprimer_thread(void * argsUnconverted)
{
  ArgImprimer * args= (ArgImprimer *)argsUnconverted;
  ecriture_log_imprimer(args->gestionnaireLog,"Lancement de la tache imprimer",ERROR);
	
  bool panneImprimante = false;
	
	
  pthread_cond_t varCond;
  pthread_mutex_t mutex;

  for (;;) {
    ecriture_log_imprimer(args->gestionnaireLog,"Tache imprimer va puller",ERROR);
    Carton cartonImpression = args->balImprimante->Pull();
    ecriture_log_imprimer(args->gestionnaireLog,"Carton arrivee - tache imprimer",ERROR);
    if(cartonImpression.fin==true)
      {
	ecriture_log_imprimer(args->gestionnaireLog,"Fin de la tache imprimer",ERROR);
	Carton c;
	c.fin=true;
	args->balPalette->Push(c,0);
	pthread_exit(0);
      }

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
	ecriture_log_imprimer(args->gestionnaireLog,"Fille d'attente pleine - tache imprimer",ERROR);
	Event msgErreurFileAttente(FILEATTPLEINE);
	args->eventBox->Push( msgErreurFileAttente,0 );
				
	// Mise en attente d'un signal envoye par le controleur pour reprendre
	pthread_mutex_lock( args->mutex );
	pthread_cond_wait( args->varCond, args->mutex );
	pthread_mutex_unlock( args->mutex );

      }

    }
    else {
      ecriture_log_imprimer(args->gestionnaireLog,"Panne imprimante - tache imprimer",ERROR);
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
