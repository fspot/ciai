#include "mutex.h"
#include <errno.h>	//def EBUSY



Mutex::Mutex()
{
	mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mut, NULL);
}

Mutex::~Mutex()
{

}

void Mutex::lock()
{
	pthread_mutex_lock(&mut);
}

bool Mutex::trylock()
{
	return (pthread_mutex_trylock(&mut) != EBUSY);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&mut);
}

pthread_mutex_t* Mutex::getMutex()
{
	return &mut;
}

