/*************************************************************************
                           stubs.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface du fichier stubs
#if ! defined ( stubs_H )
#define stubs_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "cstdlib"
//------------------------------------------------------------- Constantes 
#define PROBA_ABS_PALETTE 100	//l'absence de palette arrive 1 fois sur STUB_PROBA
#define PROBA_ERR_EMBALAGE 100
#define PROBA_ERR_PANNE_IMPRIM 100


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
bool stubPresencePalette();
bool stubErrEmbalagePalette();
bool stubPanneImprimante();

#endif // STUBS_H
