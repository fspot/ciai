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

//procédure qui permet d'écrire dans le journal interne du système Linux
void ecriture_log_remplirCarton(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


//procédure public qui permet de remplir des pièces dans un carton et de gérer tous les cas prévus
static void wait()
{
    ecriture_log_remplirCarton(init->gestionnaireLog,"Attente de deblocage - remplir Carton",ERROR);
	init->mutCv->lock();
	pthread_cond_wait(init->cv,init->mutCv->getMutex());
	init->mutCv->unlock();
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
		//on reçoit une pièce
		Piece piece=init->pBalPieces->Pull();

        ecriture_log_remplirCarton(init->gestionnaireLog,"Piece recue - remplir carton",EVENT);
        //on vérifie si la pièce lue dans la boite aux lettres n'est pas un message de pause en réalité
		if(piece.fin==true)
		{
            ecriture_log_remplirCarton(init->gestionnaireLog,"Fin de la tâche remplir carton",EVENT);
			pthread_exit(NULL);
		}

		//on vérifie qu'un carton est présent
		init->mutCartonPresent->lock();
		bool retour=(*(init->pCartonPresent));
		init->mutCartonPresent->unlock();
		if(!retour)
		{
            ecriture_log_remplirCarton(init->gestionnaireLog,"Carton abscent - remplir carton",EVENT);
			init->pBalEvenements->Push(Event(ABSCARTON),1);
			wait();
		}

		//on vérifie que la dimension de la pièce est bien celle voulue
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

		//on vérifie que le nombre de pièce dans le rebut pour un carton n'est pas
		//supérieur au seuil fixé
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

			//on vérifie si le carton est plein
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

				//on vérifie si on a terminé une série
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
