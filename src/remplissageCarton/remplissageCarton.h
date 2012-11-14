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
  Mutex* mutCartonPresent;
  bool* pCartonPresent;
  pthread_cond_t* cv;
  Mutex* mutCv;
  SharedMemoryLots* lots;
  sem_t* semLireLots;
}tInitRemplissageCarton;

void * remplirCarton(void * index);
#endif