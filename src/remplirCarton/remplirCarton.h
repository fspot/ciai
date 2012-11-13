
/*************************************************************************
                           remplirCarton  -  Tâche qui gère le remplir
*************************************************************************/

//---------- Interface de la tâche remplir carton -------
#if ! defined ( remplirCarton_H )
#define remplirCarton_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct ArgRemplirCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  pthread_mutex_t* mutCartonPresent;
  sem_t* sem_fin_de_serie;
  bool* pCartonPresent;
  Lot* lots;
  unsigned int nbLots;
  pthread_cond_t* cv;
  pthread_mutex_t* mutCv;
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void * remplirCarton(void * index);


#endif // remplirCarton_H


