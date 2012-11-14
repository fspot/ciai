#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"

#define TIME_MAX 5

#ifndef REMPLISSAGE_CARTON_H
#define REMPLISSAGE_CARTON_H
typedef struct tInitRemplissageCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  sem_t* sem_fin_de_serie;
  pthread_mutex_t* mutCartonPresent;
  bool* pCartonPresent;
  pthread_cond_t* cv;
  pthread_mutex_t* mutCv;
  SharedMemoryLots* lots;
}tInitRemplissageCarton;

void * remplirCarton(void * index);
#endif