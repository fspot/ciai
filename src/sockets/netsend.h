#ifndef NETSEND_H
#define NETSEND_H

#include <string>
#include "../mailbox/mailbox.h"

struct NetSendInitInfo {
	Mailbox<std::string> *netmb_ptr;
	int *socket_ptr;
};

void* thread_netsend(void* arg);

#endif