/*************************************************************************
                           testControleur.cpp  -  Tâche mère
                             -------------------
*************************************************************************/

//---------- Réalisation de  test Controleur ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <signal.h>
#include <time.h>

//------------------------------------------------------ Include personnel
#include "remplirCarton/remplirCarton.h"

//------------------------------------------------------ Name spaces
using namespace std;

extern bool alwaysFalse();
extern bool alwaysTrue();

///////////////////////////////////////////////////////////////////  PRIVE
//---------------------------------------------------- Variables statiques
static sem_t debutSyncro;
static pthread_t threadRemplirCarton;
static pthread_cond_t cvThreadRemplirCarton;
static Mutex mutCvRemplirCarton;
static Mailbox<Piece>* pBalPieces;
static Mailbox<Carton>* pBalCartons;
static Mailbox<Event>* pBalEvenements;
static Mailbox<Message>* pBalMessages;
static int nbTests;
static int nbMauvaisTests;
static SharedMemoryLots shMemLots; 
static ArgRemplirCarton* pArgRemplirCarton;
static Mutex mtxStandardOutput;
static Mutex mtxLotCourant;
static Log *gestionnaireLog;
static int lotCourant=0;

//------------------------------------------------------ Fonctions privées

//on teste un cas normal sans erreur
static bool test1()
{
	sem_post(&debutSyncro);
	for(int i=0; i<8;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	for (int i = 0; i < 8; i++)
	{
		sleep(1);
		Piece piece={{200,200,200},false};
		pBalPieces->Push(piece);
	}
	sleep(2);
	if(pBalCartons->Size()==9)
	{
		if(pBalEvenements->Size()==2)
		{
			if(pBalEvenements->Pull().event==FINSERIE)
			{
				if(pBalEvenements->Pull().event==FINLAST)
				{
					return true;
				}
			}
		}
	}
	return false;
}

//on teste le cas où une absence de carton est observée
static bool test2()
{
	sem_post(&debutSyncro);
	for(int i=0; i<3;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	pArgRemplirCarton->capteurCarton=alwaysTrue;
	for(int i=0; i<5;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	sleep(2);
	pArgRemplirCarton->capteurCarton=alwaysFalse;
	if(pBalCartons->Size()==1)
	{
		if(pBalEvenements->Size()==1)
		{
			if(pBalEvenements->Pull().event==ABSCARTON)
			{
				return true;
			}
		}
	}
	return false;
}

//on teste le cas où trop de pièces défectueuese par carton ont été observés
static bool test3()
{
	sem_post(&debutSyncro);
	for(int i=0; i<7;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	sleep(1);
	Piece piece={{200,200,200},false};
	pBalPieces->Push(piece);
	for(int i=0; i<8;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	sleep(2);
	if(pBalCartons->Size()==4)
	{
		if(pBalEvenements->Size()==2)
		{
			if(pBalEvenements->Pull().event==TAUXERR)
			{
				if(pBalEvenements->Pull().event==FINSERIE)
				{
					return true;
				}
			}
		}
	}
	return false;
}

//procédure qui vide toutes les boite aux lettres
static void viderBals()
{
	while(pBalEvenements->Size()>0)
	{
		pBalEvenements->Pull();
	}
	while(pBalPieces->Size()>0)
	{
		pBalPieces->Pull();
	}
	while(pBalCartons->Size()>0)
	{
		pBalCartons->Pull();
	}
	while(pBalMessages->Size()>0)
	{
		pBalMessages->Pull();
	}
}

//procédure qui réinitialise tout ce que touche le thread remplirCarton entre chaque test
static void reset()
{
	//destruction du thread remplirCarton
	pthread_cond_signal(&cvThreadRemplirCarton);
	sleep(1);
	pthread_cancel(threadRemplirCarton);
	sleep(1);

	//création du thread remplirCarton
	if(pthread_create(&threadRemplirCarton,NULL,remplirCarton,(void*)
		pArgRemplirCarton)!=0)
		cerr<<"ERROR create threadRemplirCarton"<<endl;
}

//procédure qui détruit tous les thread et objets liés aux threads
static void fin(int noSignal)
{
		pthread_cancel(threadRemplirCarton);
		sleep(1);
		pthread_cond_destroy(&cvThreadRemplirCarton);
		sem_destroy(&debutSyncro);
		delete(pBalPieces);
		delete(pBalCartons);
		delete(pBalEvenements);
		pthread_exit(NULL);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
//fonction qui retourne toujours vrai
bool alwaysTrue()
{
	return true;
}

//fonction qui retourne toujours faux
bool alwaysFalse()
{
	return false;
}

//fonction qui principal qui lance les tests
int main()
{
	//initialisation
	nbTests=0;
	nbMauvaisTests=0;

	struct sigaction action;
	action.sa_handler=fin;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;

	pthread_cond_init(&cvThreadRemplirCarton, NULL);
	sem_init(&debutSyncro,0,0);

	pBalPieces=new Mailbox<Piece>;
	pBalEvenements=new Mailbox<Event>;
	pBalCartons=new Mailbox<Carton>;
	pBalMessages=new Mailbox<Message>;

	ListeLots lots;

	//tous les tests ont ces deux deux lots à gérer
	//2 pièces par cartons
	//2 cartons par palettes
	//2 palettes par lots
	//1 pièces défectueuses dans le rebut autorisée pour un carton
	//les dimensions des pièces sont soit 100,100,100 soit 200,200,200 (x,y,z)
	lots.lots.push_back({"A",2,2,2,1,{100,100,100}});
	lots.lots.push_back({"B",2,2,2,1,{200,200,200}});

	shMemLots.content=&lots;

	gestionnaireLog=new Log(mtxStandardOutput);

	pArgRemplirCarton=new ArgRemplirCarton();
	pArgRemplirCarton->pBalPieces=pBalPieces;
	pArgRemplirCarton->pBalCartons=pBalCartons;
	pArgRemplirCarton->pBalEvenements=pBalEvenements;
	pArgRemplirCarton->pBalMessages=pBalMessages;
	pArgRemplirCarton->gestionnaireLog=gestionnaireLog;
	pArgRemplirCarton->capteurCarton=alwaysFalse;
	pArgRemplirCarton->lotCourantMutex=&mtxLotCourant;
	pArgRemplirCarton->shMemLots=&shMemLots;
	pArgRemplirCarton->lotCourant=&lotCourant;
	pArgRemplirCarton->cv=&cvThreadRemplirCarton;
	pArgRemplirCarton->mutCv=mutCvRemplirCarton.getMutex();
	pArgRemplirCarton->debutSyncro=&debutSyncro;

	sigaction(SIGINT,&action,NULL);

	//création du thread remplirCarton
	if(pthread_create(&threadRemplirCarton,NULL,remplirCarton,(void*)pArgRemplirCarton)!=0)
		cerr<<"ERROR create threadRemplirCarton"<<endl;

	//test 1
	nbTests++;
	cout<<"Debut Test "<<nbTests<<endl;
	if(!test1())
	{
		cout<<"==========>Test "<<nbTests<<": ERROR"<<endl;
		nbMauvaisTests++;
	}
	viderBals();
	reset();

	//test2
	nbTests++;
	cout<<"Debut Test "<<nbTests<<endl;	
	if(!test2())
	{
		cout<<"==========>Test "<<nbTests<<": ERROR"<<endl;
		nbMauvaisTests++;
	}
	viderBals();
	reset();

	//test3
	nbTests++;	
	cout<<"Debut Test "<<nbTests<<endl;	
	if(!test3())
	{
		cout<<"==========>Test "<<nbTests<<": ERROR"<<endl;
		nbMauvaisTests++;
	}
	viderBals();

	cout<<"Le nombre de tests réussis est de "<<nbTests-nbMauvaisTests<<"/"<<nbTests<<endl;
	fin(0);
}
