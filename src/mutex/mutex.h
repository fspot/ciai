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
		// Constructeur
		Mutex();
		//Destructeur
		~Mutex();

		//Méthode qui retourne le mutex pthread
		pthread_mutex_t* getMutex();

		//Méthode de verouillage(bloquante en cas de mutex deja verouillé)
		void lock(); 		
		//Méthode de verouillage non bloquante
		bool trylock();		
		//Méthode de deblocage du mutex
		void unlock(); 		

	private:
		pthread_mutex_t mut;
};


#endif // MUTEX_H
