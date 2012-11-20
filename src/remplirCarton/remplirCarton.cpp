/*************************************************************************
                           remplirCarton  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche remplirCarton

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
//------------------------------------------------------ Include personnel
#include "remplirCarton.h"
#include <mailbox/mailbox.h>
#include <modeles/modeles.h>
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques
static time_t timeBegin;
static ArgRemplirCarton* init;
static vector<Lot>* listeLots;
static Lot *lotCourant;
static unsigned int nbCartonsRestant;
static unsigned int nbPiecesDsRebut=0;
static unsigned int nbPiecesDsCarton;
static unsigned int idCarton;
static unsigned int serieCourante;
//------------------------------------------------------ Fonctions privées

// Méthode d'écriture dans le log
void ecriture_log_remplirCarton(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
#ifdef DEBUG
  unGestionnaire->Write(msg,unType,true);
#else
  unGestionnaire->Write(msg,unType,false);
#endif 
}

// Méthode d'attente
static void wait()
{
  ecriture_log_remplirCarton(init->gestionnaireLog,"Attente de deblocage - remplir Carton",ERROR);
  pthread_mutex_lock(init->mutCv);
  pthread_cond_wait(init->cv,init->mutCv);
  pthread_mutex_unlock(init->mutCv);
}


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void* remplirCarton(void * index)
{
  init=(ArgRemplirCarton *)index;
  ecriture_log_remplirCarton(init->gestionnaireLog,"Lancement de la tâche remplir carton",EVENT);
  serieCourante=0;
  sem_wait(init->debutSyncro);
  ecriture_log_remplirCarton(init->gestionnaireLog,"Initialisation de la tâche remplir carton finie",EVENT);
  listeLots = &init->shMemLots->content->lots;
  lotCourant=&(listeLots->at(serieCourante));	
  nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
  nbPiecesDsCarton=0;
  idCarton=0;


  for(;;)
    {
      Piece piece=init->pBalPieces->Pull();
      //ecriture_log_remplirCarton(init->gestionnaireLog,"Piece recue - remplir carton",EVENT);
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
	  ecriture_log_remplirCarton(init->gestionnaireLog,"La tâche remplir carton a detecter un carton abscent",ERROR);
	  init->pBalEvenements->Push(Event(ABSCARTON),1);
	  wait();
	}

      int i=0;
      bool valide=true;
		
      while(i<3 && valide)
	{
	  cout<<piece.dim[i]<<" "<<lotCourant->dim[i]<<endl;
	  if(piece.dim[i]!=lotCourant->dim[i])
	    {
	      valide=false;

	    }			
	  i++;
	}
		
      if(!valide)
	{
	  nbPiecesDsRebut++;
	  if(nbPiecesDsRebut>lotCourant->rebut)
	    {
	      ecriture_log_remplirCarton(init->gestionnaireLog,"Taux d'erreur trop elevé - remplir carton",EVENT);
	      init->pBalEvenements->Push(Event(TAUXERR),0);
	      wait();
	      nbPiecesDsRebut=0;
	    }
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
	      init->pBalMessages->Push(msg, 1);
	      string toSend="La tâche remplir carton a envoyé un carton(Type: "; 
		toSend+=lotCourant->nom;
		toSend+=" Nombre de pièces:"; 
		toSend+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->pieces) )->str();
		toSend+=" Taille(mm): (";
		toSend+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[0]) )->str();
		toSend+=", ";
		toSend+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[1]) )->str();
		toSend+=", "; 
		toSend+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[2]) )->str();;
		toSend+=+ ")).";
	      ecriture_log_remplirCarton(init->gestionnaireLog,toSend,EVENT);
	      nbCartonsRestant--;
	      nbPiecesDsRebut=0;
	      nbPiecesDsCarton=0;
	      if(nbCartonsRestant<=0)
		{

		  serieCourante++;

		  if((serieCourante+1)>init->shMemLots->content->lots.size())
		    {
		      string message="La tâche remplir carton a envoyé son dernier carton(Type: "; 
		      message+=lotCourant->nom;
		      message+=" Nombre de pièces:"; 
		      message+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->pieces) )->str();
		      message+=" Taille(mm): (";
		      message+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[0]) )->str();
		      message+=", ";
		      message+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[1]) )->str();
		      message+=", "; 
		      message+=static_cast<ostringstream*>( &(ostringstream() << lotCourant->dim[2]) )->str();;
		      message+=+ ")).";
		      ecriture_log_remplirCarton(init->gestionnaireLog,message,EVENT);
		      init->pBalEvenements->Push(Event(FINLAST),1);
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
		      init->pBalMessages->Push(msg, 1);
		      init->lotCourantMutex->lock();
		      (*(init->lotCourant))+=1;
		      init->lotCourantMutex->unlock();
		      init->pBalEvenements->Push(Event(FINSERIE),1);// a changer. Il faut travailler avec gestion de série mais pas avec des sémaphores
		      ecriture_log_remplirCarton(init->gestionnaireLog,"Fin d'une serie - remplir carton",EVENT);
		      lotCourant=&(init->shMemLots->content->lots[serieCourante]);
		      nbCartonsRestant=lotCourant->palettes*lotCourant->cartons;
		    }
		}
	    }
	}
    }
}
