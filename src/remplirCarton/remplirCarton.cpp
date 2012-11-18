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
static unsigned int nbPiecesDsRebut=0;
static unsigned int nbPiecesDsCarton;
static unsigned int idCarton;
static unsigned int serieCourante;


void ecriture_log_remplirCarton(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

static void wait()
{
    ecriture_log_remplirCarton(init->gestionnaireLog,"Attente de deblocage - remplir Carton",ERROR);
	pthread_mutex_lock(init->mutCv);
	pthread_cond_wait(init->cv,init->mutCv);
	pthread_mutex_unlock(init->mutCv);
}




void* remplirCarton(void * index)
{
	init=(ArgRemplirCarton *)index;
    ecriture_log_remplirCarton(init->gestionnaireLog,"Lancement de la tâche remplir carton",EVENT);
	serieCourante=0;
	sem_wait(init->debutSyncro);
    ecriture_log_remplirCarton(init->gestionnaireLog,"Initialisation finie - remplir carton",EVENT);
	listeLots = &init->shMemLots->content->lots;
	lotCourant=&(listeLots->at(serieCourante));	
	nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
	nbPiecesDsCarton=0;
	idCarton=0;


	for(;;)
	{
		Piece piece=init->pBalPieces->Pull();
        ecriture_log_remplirCarton(init->gestionnaireLog,"Piece recue - remplir carton",EVENT);
		if(piece.fin==true)
		{
            ecriture_log_remplirCarton(init->gestionnaireLog,"Fin de la tâche remplir carton",EVENT);
			pthread_exit(NULL);
		}
		init->mutCartonPresent->lock();
		bool retour=(*(init->pCartonPresent));
		init->mutCartonPresent->unlock();
		if(!retour)
		{
            ecriture_log_remplirCarton(init->gestionnaireLog,"Carton abscent - remplir carton",EVENT);
			init->pBalEvenements->Push(Event(ABSCARTON),1);
			wait();
		}

		int i=0;
		bool valide=true;
		
		while(i<3 && valide)
		{
			if(piece.dim[i]!=lotCourant->dim[i])
			{
				valide=false;
			}			
			i++;
		}
		
		if(!valide)
		{
			nbPiecesDsRebut++;
		}
		if(nbPiecesDsRebut>lotCourant->rebut)
		{
            ecriture_log_remplirCarton(init->gestionnaireLog,"Taux d'erreur trop elevé - remplir carton",EVENT);
			init->pBalEvenements->Push(Event(TAUXERR),0);
			wait();
			nbPiecesDsRebut=0;

		}
		else
		{
			nbPiecesDsCarton++;
			if(nbPiecesDsCarton>=lotCourant->pieces)
			{
				Carton carton={idCarton,lotCourant,nbPiecesDsRebut};
				init->pBalCartons->Push(carton,0);
				
				// Message réseau carton rempli :
				Message msg = {carton.netstr_rempli(), false};
				init->pBalMessages->Push(msg, 2);

				nbCartonsRestant--;
				nbPiecesDsRebut=0;
				nbPiecesDsCarton=0;
				if(nbCartonsRestant<=0)
				{

					serieCourante++;

					if((serieCourante+1)>init->shMemLots->content->lots.size())
					{
            			ecriture_log_remplirCarton(init->gestionnaireLog,"Fin de la dernière série - remplir carton",EVENT);
						init->pBalEvenements->Push(Event(FIN),1);// a changer. Il faut travailler avec gestion de série mais pas avec des sémaphores mais une bal
						Carton c;
						c.fin=true;
						init->pBalCartons->Push(c,0);
            			ecriture_log_remplirCarton(init->gestionnaireLog,"Fin de la tache remplir carton",EVENT);
						pthread_exit(NULL);
					}
					else
					{
            			// Message réseau série finie :
            			Message msg = {lotCourant->netstr(), false};
            			init->pBalMessages->Push(msg, 2);

						init->lotCourantMutex->lock();
						(*(init->lotCourant))+=1;
						init->lotCourantMutex->unlock();
                        ecriture_log_remplirCarton(init->gestionnaireLog,"Fin d'une serie - remplir carton",EVENT);
						lotCourant=&(init->shMemLots->content->lots[serieCourante]);
						nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
					}
				}
			}
		}
	}
}
