#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include "../mailbox/mailbox.h"

struct NetworkInitInfo {
	Mailbox<std::string> *netmb_ptr;
	int *socket_ptr;
};

void* thread_network(void* arg);

#endif