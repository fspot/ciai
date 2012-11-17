#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "remplirCarton.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include <iostream>
using namespace std;

static time_t timeBegin;
static ArgRemplirCarton* init;
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

void ecriture_log_remplirCarton(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


void* remplirCarton(void * index)
{
	init=(ArgRemplirCarton *)index;
        ecriture_log_remplirCarton(init->gestionnaireLog,"Lancement de la tâche remplir carton",EVENT);
	serieCourante=0;
	sem_wait(init->debutSyncro);
	init->shMemLots->mutex.lock();
	listeLots=new vector<Lot>(init->shMemLots->content->lots);
	init->shMemLots->mutex.unlock();
	lotCourant=&(listeLots->at(serieCourante));	
	nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
	nbPiecesDsCarton=0;
	idCarton=0;

	for(;;)
	{
		Piece piece=init->pBalPieces->Pull();
                ecriture_log_remplirCarton(init->gestionnaireLog,"Piece recue - rempli carton",EVENT);
		if(piece.fin==true)
		{
                        ecriture_log_remplirCarton(init->gestionnaireLog,"Fin de la tâche rempli carton",EVENT);
			pthread_exit(NULL);
		}
		init->mutCartonPresent->lock();
		bool retour=(*(init->pCartonPresent));
		init->mutCartonPresent->unlock();
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
				Carton carton={idCarton,lotCourant,nbPiecesDsRebut};
				init->pBalCartons->Push(carton,1);
				nbCartonsRestant--;
				nbPiecesDsRebut=0;
				nbPiecesDsCarton=0;
				if(nbCartonsRestant<=0)
				{
					serieCourante++;
					if((serieCourante+1)>init->shMemLots->content->lots.size())
					{
						init->pBalEvenements->Push(Event(FIN),1);// a changer. Il faut travailler avec gestion de série mais pas avec des sémaphores mais une bal
						pthread_exit(NULL);
					}
					else
					{
						lotCourant=&(init->shMemLots->content->lots[serieCourante]);
						nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
						sem_post(init->sem_fin_de_serie);
					}
				}
			}
		}
	}
}
