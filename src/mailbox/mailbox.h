/*************************************************************************
                           Mailbox  -  description
                             -------------------
*************************************************************************/

//---------- Interface de la classe Mailbox ------
#ifndef MAILBOX_H
#define MAILBOX_H
//--------------------------------------------------- Interfaces utilisées
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <utility>
#include "../multithreadObjects/mutex.h"

//------------------------------------------------------------- Constantes 
  #define PRIO_MAX 10	//nombre de niveaux de priorités, compris entre 0 et PRIO_MAX-1
//------------------------------------------------------------------ Types 

//------------------------------------------------------------------------ 
// Rôle de la classe Mailbox
// 
//	Implémente une solution de la boite au lettre à priorités
//------------------------------------------------------------------------ 


template <typename T>
class Mailbox
{

//----------------------------------------------------------------- PUBLIC

public:
	Mailbox();
	~Mailbox();

	void Push(const T& element, unsigned int priority = PRIO_MAX-1);
	//	permet d'insérer un element de type T dans la mailbox, avec la priorité priority.
	//	un message est d'autant plus prioritaire que sa priorité est petite.

	T Pull();
	//	renvoie un élément (par priorité d'abord, puis FIFO si memes priorités)
	//	bloquant si la boite aux lettres est vide.

	int Size();
	//	renvoie le nombre d'éléments dans la boite aux lettres
	//	attention quant à l'utilisation de cette valeur, du fait de la concurence elle peut ne plus être valide au moment où vous la recevez.
	//	à utiliser précautionneusement.


//------------------------------------------------------------------ PRIVE 

protected:
//----------------------------------------------------- Attributs protégés

	std::queue<T> queues[PRIO_MAX];	// queues fifo de stockage des messages, une queue par priorite
	sem_t countSemaphore;		// compte le nombre de messages actuellement dans la mailbox. Permet la lecture bloquante
	Mutex writeMutex[PRIO_MAX];	// mutexs de protection en lecture/écriture des queues.
};


#include "mailbox.ih"

#endif //MAILBOX_H
