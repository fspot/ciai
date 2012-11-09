#ifndef MAILBOX_H
#define MAILBOX_H

#define PRIO_MAX 10

#include <queue>
#include <vector>
#include <pthread.h>
#include <semaphore.h>


template <typename T>
class Mailbox
{

    typedef struct {
        unsigned int prio;
        T object;
    } prioWrapper;


// Returns true if elt1 has a higher priority than elt2
    struct comparePrio {
	bool operator()(prioWrapper& elt1, prioWrapper& elt2) const
	{
	    return (elt1.prio < elt2.prio);
	}
    };



public:
    Mailbox();
    ~Mailbox();

    void Push(const T& element, unsigned int priority);
    const T& Pull();

private:
    //methods


    //attributes
    std::priority_queue<prioWrapper,std::vector<prioWrapper>,comparePrio> content;
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
    prioWrapper pw;
    pw.prio = priority;
    pw.object = element;
    content.Push(element);

    sem_post(&countSemaphore);
}

template <typename T>
const T& Mailbox<T>::Pull()
{
    sem_wait(&countSemaphore);
    return content.Pop().object;
}

#endif