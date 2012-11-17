#ifndef NETWORK_H
#define NETWORK_H

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"

struct NetworkInitInfo {
	Mailbox<Message> *netmb_ptr;
	int *socket_ptr;
	SharedMemoryLots *shMemLots;
	sem_t * debutSyncro;
};

void* thread_network(void* arg);

#endif
