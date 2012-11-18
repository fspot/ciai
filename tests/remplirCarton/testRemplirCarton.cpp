#include <iostream>
using namespace std;
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../src/remplirCarton/remplirCarton.h"
#include <string>
#include <signal.h>
#include <time.h>

static bool cartonPresent=true;
static Mutex* mutCartonPresent;
static sem_t sem_fin_de_serie;
static sem_t debutSyncro;
static pthread_t threadRemplirCarton;
static pthread_cond_t cvThreadRemplirCarton;
static Mutex* mutCvRemplirCarton;
static Mailbox<Piece>* pBalPieces;
static Mailbox<Carton>* pBalCartons;
static Mailbox<Event>* pBalEvenements;
static int nbTests;
static int nbMauvaisTests;
static SharedMemoryLots shMemLots; 
static ArgRemplirCarton* pArgRemplirCarton;
static Mutex mtxStandardOutput;
static Mutex mtxLotCourant;
static Log *gestionnaireLog;

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
		if(pBalEvenements->Size()==1)
		{
			if(pBalEvenements->Pull().event==FIN)
			{
				return true;
			}
		}
	}
	return false;
}

static bool test2()
{
	sem_post(&debutSyncro);
	for(int i=0; i<3;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	mutCartonPresent->lock();
	cartonPresent=false;
	mutCartonPresent->unlock();
	for(int i=0; i<5;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	sleep(2);
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
	for(int i=0; i<3;i++)
	{
		sleep(1);
		Piece piece={{100,100,100},false};
		pBalPieces->Push(piece);
	}
	sleep(2);
	if(pBalCartons->Size()==4)
	{
		if(pBalEvenements->Size()==1)
		{
			if(pBalEvenements->Pull().event==TAUXERR)
			{
				return true;
			}
		}
	}
	return false;
}



static void reset()
{
	pthread_cancel(threadRemplirCarton);
	sleep(1);
	pthread_cond_destroy(&cvThreadRemplirCarton);
	delete(mutCartonPresent);
	delete(mutCvRemplirCarton);
	sem_destroy(&sem_fin_de_serie);
	sem_destroy(&debutSyncro);
	delete(pBalPieces);
	delete(pBalCartons);
	delete(pBalEvenements);
	delete(pArgRemplirCarton);

	sem_init(&sem_fin_de_serie,0,0);
	sem_init(&debutSyncro,0,0);
	pthread_cond_init(&cvThreadRemplirCarton, NULL);
	cartonPresent=true;
	mutCartonPresent=new Mutex();
	mutCvRemplirCarton=new Mutex();
	pBalPieces=new Mailbox<Piece>;
	pBalEvenements=new Mailbox<Event>;
	pBalCartons=new Mailbox<Carton>;
	pArgRemplirCarton=new ArgRemplirCarton(pBalPieces,pBalCartons,pBalEvenements,gestionnaireLog,mutCartonPresent,
		&sem_fin_de_serie,&cartonPresent,&mtxLotCourant,&shMemLots,0,NULL,&cvThreadRemplirCarton,
		mutCvRemplirCarton,&debutSyncro);
	if(pthread_create(&threadRemplirCarton,NULL,remplirCarton,(void*)
		pArgRemplirCarton)!=0)
		cerr<<"ERROR create threadRemplirCarton"<<endl;
}

static void fin(int noSignal)
{
	if(noSignal==SIGINT)
	{
		pthread_cancel(threadRemplirCarton);
		sleep(1);
		pthread_cond_destroy(&cvThreadRemplirCarton);
		delete(mutCartonPresent);
		delete(mutCvRemplirCarton);
		sem_destroy(&sem_fin_de_serie);
		sem_destroy(&debutSyncro);
		delete(pBalPieces);
		delete(pBalCartons);
		delete(pBalEvenements);
		pthread_exit(NULL);
	}
}

int main()
{
	nbTests=0;
	nbMauvaisTests=0;

	struct sigaction action;
	action.sa_handler=fin;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;

	pthread_cond_init(&cvThreadRemplirCarton, NULL);
	mutCartonPresent=new Mutex();
	mutCvRemplirCarton=new Mutex();
	sem_init(&sem_fin_de_serie,0,0);
	sem_init(&debutSyncro,0,0);

	pBalPieces=new Mailbox<Piece>;
	pBalEvenements=new Mailbox<Event>;
	pBalCartons=new Mailbox<Carton>;

	ListeLots lots;
	lots.lots.push_back({"A",2,2,2,1,{100,100,100}});
	lots.lots.push_back({"B",2,2,2,1,{200,200,200}});

	shMemLots.content=&lots;

	gestionnaireLog=new Log(mtxStandardOutput);

	pArgRemplirCarton=new ArgRemplirCarton(pBalPieces,pBalCartons,pBalEvenements,gestionnaireLog,mutCartonPresent,
		&sem_fin_de_serie,&cartonPresent,&mtxLotCourant,&shMemLots,0,NULL,&cvThreadRemplirCarton,
		mutCvRemplirCarton,&debutSyncro);

	sigaction(SIGINT,&action,NULL);

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
	reset();

	//test2
	nbTests++;
	cout<<"Debut Test "<<nbTests<<endl;	
	if(!test2())
	{
		cout<<"==========>Test "<<nbTests<<": ERROR"<<endl;
		nbMauvaisTests++;
	}
	reset();

	//test3
	nbTests++;	
	cout<<"Debut Test "<<nbTests<<endl;	
	if(!test3())
	{
		cout<<"==========>Test "<<nbTests<<": ERROR"<<endl;
		nbMauvaisTests++;
	}
	reset();

	cout<<"Le nombre de tests réussis est de "<<nbTests-nbMauvaisTests<<"/"<<nbTests<<endl;
	fin(SIGINT);
}