/*************************************************************************
                           mutex.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la classe mutex
#if ! defined ( MUTEX_H )
#define MUTEX_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

#include "pthread.h"


class Mutex
{
	public:
		Mutex();
		~Mutex();

		pthread_mutex_t* getMutex();//retourne le mutex

		void lock(); 		// prendre le mutex (peut être bloquant)
		bool trylock();		// essaye de prendre le mutex, renvoie true si ok. (jamais bloquant)
		void unlock(); 		// rend le mutex.

	private:
		pthread_mutex_t mut;
};


#endif // MUTEX_H
