#ifndef NETSEND_H
#define NETSEND_H

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

struct NetSendInitInfo {
	Mailbox<Message> *netmb_ptr;
	int *socket_ptr;
  	Log * gestionnaireLog;
};

void* thread_netsend(void* arg);

#endif
