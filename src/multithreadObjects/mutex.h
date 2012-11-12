#include "pthread.h"

class Mutex
{
	public:
		Mutex();
		~Mutex();
		
		void lock(); // prendre le mutex (peut être bloquant)
		void unlock(); // rend le mutex.
		
	private:
		pthread_mutex_t mut;
};

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