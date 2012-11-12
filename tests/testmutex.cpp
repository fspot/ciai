#include <iostream>
#include <string>
#include <pthread.h>

#include "mutex.h"

#define MAX 100000

using namespace std;

void* thread_a(void* arg)
{
	Mutex *m = (Mutex*) arg;
	m->lock();
	cout << "<A says> : J'ai le mutex !" << endl;
	for (int i=0 ; i<MAX ; i++) {
		cout << "A : " << i << endl;
	}
	m->unlock();
	pthread_exit(0);
}

void* thread_b(void *arg)
{
	Mutex *m = (Mutex*) arg;
	m->lock();
	cout << "<B says> : J'ai le mutex !" << endl;
	for (int i=0 ; i<MAX ; i++) {
		cout << "B : " << i << endl;
	}
	m->unlock();
	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	pthread_t a, b;
	void *ret;

	Mutex mut; // le mutex

	// lancement du thread
	cout << "## main is doing some stuff ... creating new thread .." << endl;
	pthread_create (&a, NULL, thread_a, (void*) &mut);
	pthread_create (&b, NULL, thread_b, (void*) &mut);
	cout << "## main is waiting other threads to finish..." << endl;
	pthread_join(a, &ret);
	pthread_join(b, &ret);
	
    return 0;
}