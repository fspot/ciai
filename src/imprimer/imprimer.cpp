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

    pthread_cond_t varCond;
    pthread_mutex_t mutex;

    for (;;) {
	// Attente passive sur la boite aux lettres
	Carton cartonImpression = args->balImprimante->Pull();

	if(cartonImpression.fin==true)
	{
	    ecriture_log_imprimer(args->gestionnaireLog,"Fin de la tache imprimer",ERROR);
	    Carton c;
	    c.fin=true;
	    args->balPalette->Push(c,0);
	    pthread_exit(0);
	}
    	ecriture_log_imprimer(args->gestionnaireLog,"La tache imprimer a recu un carton",EVENT);

	if (args->capteurPanne()) {
	    ecriture_log_imprimer(args->gestionnaireLog,"Une panne imprimante est survenue",ERROR);
	    // Depot message erreur dans la bal eventbox
	    Event msgErreurImp(PANNEIMPRIM);
	    args->eventBox->Push( msgErreurImp ,0);
		
	    // Mise en attente d'un signal envoye par le controleur pour reprendre
	    pthread_mutex_lock( args->mutex );
	    pthread_cond_wait( args->varCond, args->mutex );
	    pthread_mutex_unlock( args->mutex );
	}

	if (args->balPalette->Size() >= 10) {

	    // Depot message erreur dans la bal enventbox
	    ecriture_log_imprimer(args->gestionnaireLog,"La tache imprimer a detecter une fille d'attente pleine",ERROR);
	    Event msgErreurFileAttente(FILEATTPLEINE);
	    args->eventBox->Push( msgErreurFileAttente,0 );
				
	    // Mise en attente d'un signal envoye par le controleur pour reprendre
	    pthread_mutex_lock( args->mutex );
	    pthread_cond_wait( args->varCond, args->mutex );
	    pthread_mutex_unlock( args->mutex );
	}

	// Depot Carton dans la bal balPalette
	string message;
	message=+"La tache imprimer a imprimé et transmit un carton";
	ecriture_log_imprimer(args->gestionnaireLog,message,ERROR);
	args->balPalette->Push(cartonImpression,0);
    }
}
