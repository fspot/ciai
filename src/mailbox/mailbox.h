#ifndef MAILBOX_H
#define MAILBOX_H

#define PRIO_MAX 10

#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <utility>
#include <stdio.h>



template <typename T>
class Mailbox
{
	
public:
	Mailbox();
	~Mailbox();
	
	void Push(const T& element, unsigned int priority);
	//	permet d'insérer un element de type T dans la mailbox, avec la priorité priority.
	//	un message est d'autant plus prioritaire que sa priorité est petite.
	
	T Pull();
	//	renvoie un élément (par priorité d'abord, puis FIFO si memes priorités)
	//	bloquant si la boite aux lettres est vide.
	
private:
	//methods
	
	
	//attributes
	std::queue<T> queues[PRIO_MAX];
	sem_t countSemaphore;
};




/////////////////
// mailbox.cpp //
/////////////////

template <typename T>
Mailbox<T>::Mailbox()
{
	sem_init(&countSemaphore, 0, 0);
}

template <typename T>
Mailbox<T>::~Mailbox()
{
	sem_destroy(&countSemaphore);
}

template <typename T>
void Mailbox<T>::Push(const T& element, unsigned int priority)
{
	if (priority >= PRIO_MAX)
	{
		std::cout << "[WARNING] Mailbox::Push() - priority higher than PRIO_MAX (set back to " << PRIO_MAX-1 << ")" << std::endl;
		priority = PRIO_MAX-1;
	}
	
	queues[priority].push(element);
	
	sem_post(&countSemaphore);
}

template <typename T>
T Mailbox<T>::Pull()
{
	sem_wait(&countSemaphore);
	for (unsigned int i=0 ; true ; i++ )
	{
		if (queues[i].size())
		{
			T returnedElt = queues[i].front();
			queues[i].pop();
			return returnedElt;
		}
	}
}


#endif //MAILBOX_H