#ifndef NETSEND_H
#define NETSEND_H

#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"

struct NetSendInitInfo {
	Mailbox<Message> *netmb_ptr;
	int *socket_ptr;
};

void* thread_netsend(void* arg);

#endif
