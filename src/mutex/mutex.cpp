/*************************************************************************
                           Mutex  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la classe mutex

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include "mutex.h"
#include <errno.h>	
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Méthodes publiques

//Contructeur par défaut
Mutex::Mutex()
{
    mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&mut, NULL);
}


// Destructeur par défaut
Mutex::~Mutex()
{
    pthread_mutex_destroy(&mut);
}

//Méthode de vérouillage
void Mutex::lock()
{
    pthread_mutex_lock(&mut);
}

//Méthode de tentative de vérouillage
bool Mutex::trylock()
{
    return (pthread_mutex_trylock(&mut) != EBUSY);
}

// Méthode de dévérouillage
void Mutex::unlock()
{
    pthread_mutex_unlock(&mut);
}

// Récupération du mutex
pthread_mutex_t* Mutex::getMutex()
{
    return &mut;
}

