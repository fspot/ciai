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
#define PROBA_ABS_PALETTE 500	//l'absence de palette arrive 1 fois sur STUB_PROBA
#define PROBA_ERR_EMBALAGE 500
#define PROBA_ERR_PANNE_IMPRIM 500
#define PROBA_ERR_CARTON 500


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
bool stubPresencePalette();
bool stubErrEmbalagePalette();
bool stubPanneImprimante();
bool stubAbscenceCarton();

#endif // STUBS_H
