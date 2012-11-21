/*************************************************************************
                           stubs  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation du fichier stubs

/////////////////////////////////////////////////////////////////  INCLUDE
//------------------------------------------------------ Include personnel
#include "stubs.h"
///////////////////////////////////////////////////////////////////  PRIVE

using namespace std;

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// Méthode de génération de présence palette
bool stubPresencePalette()
{
#ifdef ABS_PALETTE
    return false;
#else
    return (rand() % PROBA_ABS_PALETTE == 0);
#endif
}


// Méthode de génération de problème d'emblaage
bool stubErrEmbalagePalette()
{
#ifdef ERR_EMBALAGE
    return true;
#else 
    return (rand() % PROBA_ERR_EMBALAGE == 0);
#endif
}

bool stubPanneImprimante()
{
#ifdef ERR_IMPRIM
    return true;
#else 
    return (rand() % PROBA_ERR_PANNE_IMPRIM == 0);
#endif
}


bool stubAbscenceCarton()
{
#ifdef ERR_CARTON
    return true;
#else 
    return (rand() % PROBA_ERR_CARTON == 0);
#endif
}
