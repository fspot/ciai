#ifndef REMPLIRPALETTE_H
#define REMPLIRPALETTE_H
#include "../stubs/stubs.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include <pthread.h>

struct remplirPaletteArgs{
  Mailbox<Carton>* balImprimante;
  Mailbox<Palette>* balPalette;
  Mailbox<Event>* eventBox;
  sem_t* afterErrorRestart;
  SharedMemoryLots* shMemLots;
};






#endif //REMPLIRPALETTE_H