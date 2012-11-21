#ifndef NETWORK_H
#define NETWORK_H

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

struct NetworkInitInfo {
	Mailbox<Event> * balEvenements;
	Mailbox<Message> *netmb_ptr;
	Mailbox<ListeCommandes> *balCommandes;
	int *socket_ptr;
	SharedMemoryLots *shMemLots;
	sem_t * debutSyncro;
  	Log * gestionnaireLog;
};

// thread tâche réception
void* thread_network(void* arg);

#endif
