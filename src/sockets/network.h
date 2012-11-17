#ifndef NETWORK_H
#define NETWORK_H

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

struct NetworkInitInfo {
	Mailbox<Message> *netmb_ptr;
	int *socket_ptr;
	SharedMemoryLots *shMemLots;
<<<<<<< HEAD
	sem_t * debutSyncro;
=======
  	Log * gestionnaireLog;
>>>>>>> d9d6d02c8e54bc521077efd099614d16b4572eeb
};

void* thread_network(void* arg);

#endif
