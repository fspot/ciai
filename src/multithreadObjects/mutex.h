#ifndef MUTEX_H
#define MUTEX_H

#include "pthread.h"

class Mutex
{
	public:
		Mutex();
		~Mutex();

		pthread_mutex_t* getMutex();

		void lock(); // prendre le mutex (peut être bloquant)
		void unlock(); // rend le mutex.

	private:
		pthread_mutex_t mut;
};



#endif