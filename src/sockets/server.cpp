#include <iostream>
#include <pthread.h>

#include "network.h"

using namespace std;

int main(int argc, char* argv[])
{
	pthread_t network;
	void *ret;
	
	cout << "## main is doing some stuff ... creating new thread .." << endl;
	if (pthread_create (&network, NULL, thread_network, NULL) < 0)
	{
		cerr << "pthread_create error for thread network" << endl;
		return 1;
	}

	cout << "## main is waiting the other thread to finish..." << endl;
	pthread_join(network, &ret);
	
    return 0;
}

