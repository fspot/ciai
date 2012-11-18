#include <iostream>
#include <vector>
#include <map>

#include "pthread.h"

#include "piece.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std; 

void ecriture_log_piece(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}

void *thread_piece(void * argPiece)
{
	srand ( time(NULL) );
	ArgPiece *arg = (ArgPiece*) argPiece; // cast
        ecriture_log_piece(arg->gestionnaireLog, "Lancement de la tâche pièce", EVENT);

	sem_wait(arg->debutSyncro);

	// boucle infinie, on sera killé par la tâche mère.
	while(1)
	{
		if (arg->clapet->trylock() == false) { // si le clapet est fermé...
			arg->clapet->lock(); // ... j'attend qu'il s'ouvre.
			arg->clapet->unlock();
		}
		arg->clapet->unlock();
		// le clapet est ouvert => je génère une pièce :
		Piece p;
		arg->lotCourantMutex->lock();
		int i= *(arg->lotCourant);
		arg->lotCourantMutex->unlock();
		arg->shMemLots->mutex.lock();
		for(int j=0;j<3;j++)
		{
			int v1 = rand() % 100;
			int y=0;
			if (v1<2)
			{
				y=1;
			}
			p.dim[j]=arg->shMemLots->content->lots[i].dim[j]+y;
		}		
		arg->shMemLots->mutex.unlock();
		arg->balPiece->Push(p,0);
		cout<<"je dépose"<<endl;
		// pause :
		sleep(1);
	}
}
