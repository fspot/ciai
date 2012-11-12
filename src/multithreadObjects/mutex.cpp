
#include "mutex.h"

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

void Mutex::unlock()
{
	pthread_mutex_unlock(&mut);
}

pthread_mutex_t* Mutex::getMutex()
{
	return &mut;
}

