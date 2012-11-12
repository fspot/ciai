#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "remplissageCarton.h"
#include "structs.h"
#include <iostream>
using namespace std;

#define TIME_MAX 3 //en secondes
#define ERREUR_CARTON_ABSENT 1
#define ERREUR_TROP_PIECES_DEFECTUEUSES 2
#define ERREUR_PIECE_ABSENTE 3
#define FIN 4

static time_t timeBegin;
static tInitRemplissageCarton* init;
static tLot *lotCourant;
static unsigned int nbCartonsRestant;
static unsigned int nbPiecesDsRebut;
static unsigned int nbPiecesDsCarton;
static unsigned int idCarton;
static unsigned int serieCourante;
static pthread_mutex_t mutCV;

static void remplirCartonReel(int noSignal)
{
	cout<<"heu1"<<endl;
	pthread_mutex_lock(init->mutCartonPresent);
	bool retour=(*(init->pCartonPresent));
	pthread_mutex_unlock(init->mutCartonPresent);
	if(!retour)
	{
		cout<<"heu2"<<endl;
		init->pBalEvenements->write(1,tEvenement(ERREUR_CARTON_ABSENT));
		pthread_mutex_lock(&mutCV);
		pthread_cond_wait(init->cv,&mutCV);
		pthread_mutex_unlock(&mutCV);
	}

	tPiece piece=init->pBalPieces->read();

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

	if(nbPiecesDsRebut>lotCourant->nbRebut)
	{
		cout<<"heu3"<<endl;
		init->pBalEvenements->write(1,tEvenement(ERREUR_TROP_PIECES_DEFECTUEUSES));
		pthread_mutex_lock(&mutCV);
		pthread_cond_wait(init->cv,&mutCV);
		pthread_mutex_unlock(&mutCV);
	}
	else
	{
		nbPiecesDsCarton++;
		if(nbPiecesDsCarton>=lotCourant->nbPieces)
		{
			cout<<"heu4"<<endl;
			nbPiecesDsCarton=0;
			init->pBalCartons->write(1,tCarton(idCarton));
			nbCartonsRestant--;
			if(nbCartonsRestant<=0)
			{
				cout<<"heu5"<<endl;
				serieCourante++;
				if((serieCourante+1)>init->nbLots)
				{
					cout<<"heu6"<<endl;
					init->pBalEvenements->write(1,tEvenement(FIN));
					pthread_mutex_lock(&mutCV);
					pthread_cond_wait(init->cv,&mutCV);
					pthread_mutex_unlock(&mutCV);
				}
				else
				{
					cout<<"heu7"<<endl;
					lotCourant=&(init->lots[serieCourante]);
					nbCartonsRestant=lotCourant->nbPalettes*lotCourant->nbCartons;
					sem_post(init->sem_fin_de_serie);
				}
			}
		}
	}
	time(&timeBegin);
}

void* remplirCarton(void * index)
{
	struct sigaction action;
	action.sa_handler=remplirCartonReel;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;

	pthread_mutex_init(&mutCV,NULL);

	init=(tInitRemplissageCarton *)index;
	serieCourante=0;
	lotCourant=&(init->lots[serieCourante]);	
	nbCartonsRestant=lotCourant->nbPalettes*lotCourant->nbCartons;
	nbPiecesDsCarton=0;
	idCarton=0;

	sigaction(SIGUSR1,&action,NULL);

	time(&timeBegin);

	for(;;)
	{
		sleep(1);
		if(difftime(time(NULL),timeBegin)>TIME_MAX)
		{
			cout<<"heu8"<<endl;
			init->pBalEvenements->write(1,tEvenement(ERREUR_PIECE_ABSENTE));
			pthread_mutex_lock(&mutCV);
			pthread_cond_wait(init->cv,&mutCV);
			pthread_mutex_unlock(&mutCV);
		}
	}
}