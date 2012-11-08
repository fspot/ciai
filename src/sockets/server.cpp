#include <iostream>
#include <pthread.h>

#include "network.h"
#include "netsend.h"

using namespace std;

int main(int argc, char* argv[])
{
	pthread_t network, netsend;
	void *ret;

	int socket; // cette variable contiendra le socket utile aux 2 threads.
	
	cout << "## main is doing some stuff ... creating new thread .." << endl;
	if (pthread_create (&network, NULL, thread_network, (void*) &socket) < 0)
	{
		cerr << "pthread_create error for thread network" << endl;
		return 1;
	}

	if (pthread_create (&netsend, NULL, thread_netsend, (void*) &socket) < 0)
	{
		cerr << "pthread_create error for thread netsend" << endl;
		return 1;
	}

	cout << "## main is waiting the other thread to finish..." << endl;
	pthread_join(network, &ret);
	pthread_join(netsend, &ret);
	
    return 0;
}

