#ifndef REMPLISSAGE_CARTON_H
#define REMPLISSAGE_CARTON_H
#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
struct ArgRemplissageCarton{
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


void * remplirCarton(void * index);
#endif
