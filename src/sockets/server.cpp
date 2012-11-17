#include <iostream>
#include <string>
#include <pthread.h>

#include "network.h"
#include "netsend.h"
#include "../mailbox/mailbox.h"

using namespace std;

void* thread_test(void* arg)
{
	Mailbox<Message> *netmb = (Mailbox<Message>*) arg;
	sleep(10);

	for (int i=0 ; i<10 ; i++) {
		Message m;
		m.contenu = "HEY_";
		netmb->Push(m, 2);
		sleep(1);
	}

	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	pthread_t network, netsend;
	void *ret;

	int socket; // cette variable contiendra le socket utile aux 2 threads.
	Mailbox<Message> netmb; // celle ci est la Mailbox pour l'envoi réseau.

	// argument pour le thread_network
	NetworkInitInfo network_arg;
	network_arg.netmb_ptr = &netmb;
	network_arg.socket_ptr = &socket;
	// lancement du thread
	cout << "## main is doing some stuff ... creating new thread .." << endl;
	if (pthread_create (&network, NULL, thread_network, (void*) &network_arg) < 0)
	{
		cerr << "pthread_create error for thread network" << endl;
		return 1;
	}

	// argument pour le thread_netsend
	NetSendInitInfo netsend_arg;
	netsend_arg.netmb_ptr = &netmb;
	netsend_arg.socket_ptr = &socket;
	// lancement du thread
	if (pthread_create (&netsend, NULL, thread_netsend, (void*) &netsend_arg) < 0)
	{
		cerr << "pthread_create error for thread netsend" << endl;
		return 1;
	}

	// test :
	/*
	pthread_t test;
	pthread_create (&test, NULL, thread_test, (void*) &netmb);
	pthread_join(test, &ret);
	/**/

	cout << "## main is waiting other threads to finish..." << endl;
	pthread_join(network, &ret);
	pthread_join(netsend, &ret);
	
    return 0;
}

