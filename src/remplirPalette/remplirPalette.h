#ifndef REMPLIRPALETTE_H
#define REMPLIRPALETTE_H
#include "../stubs/stubs.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include "../log/log.h" 
#include <pthread.h>

struct ArgsRemplirPalette{
  Mailbox<Carton>* balImprimante;
  Log * gestionnaireLog;
  Mailbox<Palette>* balPalette;
  Mailbox<Event>* eventBox;
  pthread_cond_t *cw;
  pthread_mutex_t* mxcw;
  SharedMemoryLots* shMemLots;
  sem_t * debutSyncro;
};

void remplirPalette_thread(void * args);




#endif //REMPLIRPALETTE_H
