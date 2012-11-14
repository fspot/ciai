#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "remplissageCarton.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include <iostream>
using namespace std;

static time_t timeBegin;
static tInitRemplissageCarton* init;
static vector<Lot>* listeLots;
static Lot *lotCourant;
static unsigned int nbCartonsRestant;
static unsigned int nbPiecesDsRebut;
static unsigned int nbPiecesDsCarton;
static unsigned int idCarton;
static unsigned int serieCourante;

static void wait()
{
	pthread_mutex_lock(init->mutCv);
	pthread_cond_wait(init->cv,init->mutCv);
	pthread_mutex_unlock(init->mutCv);
}

void* remplirCarton(void * index)
{
	init=(tInitRemplissageCarton *)index;
	serieCourante=0;

	init->lots->mutex.lock();
	listeLots=new vector<Lot>(init->lots->content->lots);
	init->lots->mutex.unlock();
	lotCourant=&(listeLots->at(serieCourante));	
	nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
	nbPiecesDsCarton=0;
	idCarton=0;

	for(;;)
	{
		Piece piece=init->pBalPieces->Pull();
		if(piece.fin=true)
			pthread_exit(NULL);
		
		pthread_mutex_lock(init->mutCartonPresent);
		bool retour=(*(init->pCartonPresent));
		pthread_mutex_unlock(init->mutCartonPresent);
		if(!retour)
		{
			init->pBalEvenements->Push(Event(ABSCARTON),1);
			wait();
		}

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
			init->pBalEvenements->Push(Event(TAUXERR),1);
			wait();
		}
		else
		{
			nbPiecesDsCarton++;
			if(nbPiecesDsCarton>=lotCourant->pieces)
			{
				nbPiecesDsCarton=0;
				Carton carton={idCarton,lotCourant,nbPiecesDsRebut};
				init->pBalCartons->Push(carton,1);
				nbCartonsRestant--;
				if(nbCartonsRestant<=0)
				{
					serieCourante++;
					if((serieCourante+1)>init->lots->content->lots.size())
					{
						init->pBalEvenements->Push(Event(FIN),1);// a changer. Il faut travailler avec gestion de série mais pas avec des sémaphores mais une bal
						wait();
					}
					else
					{
						lotCourant=&(init->lots->content->lots[serieCourante]);
						nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
						sem_post(init->sem_fin_de_serie);
					}
				}
			}
		}
	}
}