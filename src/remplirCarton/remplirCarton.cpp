#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "remplirCarton.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include <iostream>
using namespace std;

#define TIME_MAX 10

static time_t timeBegin;
static ArgRemplirCarton* init;
static Lot *lotCourant;
static unsigned int nbCartonsRestant;
static unsigned int nbPiecesDsRebut;
static unsigned int nbPiecesDsCarton;
static unsigned int idCarton;
static unsigned int serieCourante;

void wait()
{
	pthread_mutex_lock(init->mutCv);
	pthread_cond_wait(init->cv,init->mutCv);
	pthread_mutex_unlock(init->mutCv);
}

static void remplirCartonReel(int noSignal)
{
	if(noSignal==SIGUSR1)
	{
		cout<<"heu1"<<endl;
		pthread_mutex_lock(init->mutCartonPresent);
		bool retour=(*(init->pCartonPresent));
		pthread_mutex_unlock(init->mutCartonPresent);
		if(!retour)
		{
			cout<<"heu2"<<endl;
			init->pBalEvenements->Push(Event(ABSCARTON),1);
			wait();
		}

		Piece piece=init->pBalPieces->Pull();

		int i=0;
		bool valide=true;
		while(i<3 && valide)
		{
			if(piece.dim[i]!=lotCourant->dim[i])
				valide=false;
			i++;
		}

		if(!valide)
			nbPiecesDsRebut++;

		if(nbPiecesDsRebut>lotCourant->rebut)
		{
			cout<<"heu3"<<endl;
			init->pBalEvenements->Push(Event(TAUXERR),1);
			wait();
		}
		else
		{
			nbPiecesDsCarton++;
			if(nbPiecesDsCarton>=lotCourant->pieces)
			{
				cout<<"heu4"<<endl;
				nbPiecesDsCarton=0;
				Carton carton={idCarton,lotCourant,nbPiecesDsRebut};
				init->pBalCartons->Push(carton,1);
				nbCartonsRestant--;
				if(nbCartonsRestant<=0)
				{
					cout<<"heu5"<<endl;
					serieCourante++;
					if((serieCourante+1)>init->nbLots)
					{
						cout<<"heu6"<<endl;
						init->pBalEvenements->Push(Event(FIN),1);// a changer. Il faut travailler avec gestion de série mais pas avec des sémaphores mais une bal
						wait();
					}
					else
					{
						cout<<"heu7"<<endl;
						lotCourant=&(init->lots[serieCourante]);
						nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
						sem_post(init->sem_fin_de_serie);
					}
				}
			}
		}
	}
	else
	{
		wait();
	}
	time(&timeBegin);
}

void* remplirCarton(void * index)
{
	init=(ArgRemplirCarton *)index;
	serieCourante=0;
	//lotCourant=&(init->lots[serieCourante]);	
	//nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
	nbPiecesDsCarton=0;
	idCarton=0;

	signal(SIGUSR1,remplirCartonReel);
	signal(SIGUSR2,remplirCartonReel);

	time(&timeBegin);


	for(;;)
	{
		sleep(1);
		if(difftime(time(NULL),timeBegin)>TIME_MAX)
		{
			cout<<"heu8"<<endl;
			init->pBalEvenements->Push(Event(ABSPIECE),1);
			wait();
		}
	}
}
