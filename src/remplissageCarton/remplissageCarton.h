

#define TIME_MAX 5

typedef struct tInitRemplissageCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  pthread_mutex_t* mutCartonPresent;
  sem_t* sem_fin_de_serie;
  bool* pCartonPresent;
  pthread_cond_t* cv;
  pthread_mutex_t* mutCv;
  pthread_mutex_t mutBegin;
  TsharedMemory* lots;
}tInitRemplissageCarton;

#ifndef REMPLISSAGE_CARTON_H
#define REMPLISSAGE_CARTON_H
void * remplirCarton(void * index);
#endif