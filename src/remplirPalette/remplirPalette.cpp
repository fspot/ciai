/*************************************************************************
                           remplirPalette  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche remplirPalette

/////////////////////////////////////////////////////////////////  INCLUDE
//------------------------------------------------------ Include personnel
#include "remplirPalette.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées


// Méthode d'écriture dans le log
void ecriture_log_remplirPalette(Log * unGestionnaire, std::string msg,logType unType)
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
//Méthode de thread
void remplirPalette_thread(void * argsUncasted)
{
    static unsigned int idpalette = 0;
    ArgsRemplirPalette * args = (ArgsRemplirPalette *) argsUncasted;
    ecriture_log_remplirPalette(args->gestionnaireLog,"Lancement de la tache remplir palette",EVENT);
    Palette palette;
    int countCarton = 0, countPalette = 0, countLot = 0;

    while (1)
    {
    	Carton carton = args->balPalette->Pull(); // appel bloquant

    	if (carton.fin)
        {
		ecriture_log_remplirPalette(args->gestionnaireLog,"Fin de la tache remplir palette",EVENT);
		Palette p;
		p.fin=true;
    		args->balStockage->Push(p,0);
    		pthread_exit(0);
	}

	ecriture_log_remplirPalette(args->gestionnaireLog,"La tache remplir palette a recu un carton de type "+args->shMemLots->content->lots[countLot].nom,EVENT);
	// Message réseau carton empaletté :
	Message msg = {carton.netstr_palette(), false};
	args->balMessages->Push(msg, 1);

    	// passage au carton suivant :
    	countCarton++;		
        ecriture_log_remplirPalette(args->gestionnaireLog,"Le carton a été empilé par la tache remplir palette",EVENT);
    	if (countCarton == args->shMemLots->content->lots[countLot].cartons) { // next palette
	    	// on emballe la palette qu'on vient de finir :
    		if ( (*args->capteurEmbalage)() ) 
		{ // si erreur emballage :
			ecriture_log_remplirPalette(args->gestionnaireLog,"La tache remplir palette a eu une erreur en emballant",EVENT);
    			args->eventBox->Push(Event(ABSPALETTE),0);
    			pthread_mutex_lock(args->mxcw);
    			pthread_cond_wait(args->cw,args->mxcw);
    			pthread_mutex_unlock(args->mxcw);
    		}

    		// On donne un id et un id_lot à la palette finie :
    		palette.id = idpalette++;
    		args->shMemLots->mutex.lock();
    		palette.lot = &args->shMemLots->content->lots[countLot];
    		args->shMemLots->mutex.unlock();


		ecriture_log_remplirPalette(args->gestionnaireLog,"La tache remplir palette a empilé le carton",EVENT);
    		// on push la palette
    		args->balStockage->Push(palette,0);

    		// Message réseau palette finie :
    		Message msg = {palette.netstr(), false};
    		args->balMessages->Push(msg, 1);


    		countCarton = 0;
	    	countPalette++;
	    	if (countPalette == args->shMemLots->content->lots[countLot].palettes) { // next lot
	    		countPalette = 0;
	    		countLot++;
	    		// test de fin de production :
	    		if (countLot == args->shMemLots->content->lots.size())
	    			break;
	    	}

	    	// la palette est elle bien présente ?
	    	if (! (*args->capteurPalette)()) 
		{ // palette absente !
			ecriture_log_remplirPalette(args->gestionnaireLog,"La tache remplir palette a detecté l'abscence d'une palette",EVENT);
	    		args->eventBox->Push(Event(ERREMBALAGES),0);
	    		pthread_mutex_lock(args->mxcw);
	    		pthread_cond_wait(args->cw,args->mxcw);
	    		pthread_mutex_unlock(args->mxcw);
	    	}
	    }
    }
    pthread_exit(0);
}

