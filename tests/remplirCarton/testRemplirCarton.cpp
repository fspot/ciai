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
static sem_t debutSyncro;
static pthread_t threadRemplirCarton;
static pthread_cond_t cvThreadRemplirCarton;
static Mutex* mutCvRemplirCarton;
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
				if(pBalEvenements->Pull().event==FIN)
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
	cartonPresent=true;
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
	pthread_cond_signal(&cvThreadRemplirCarton);
	sleep(1);
	pthread_cancel(threadRemplirCarton);
	sleep(1);

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
	sem_init(&debutSyncro,0,0);

	pBalPieces=new Mailbox<Piece>;
	pBalEvenements=new Mailbox<Event>;
	pBalCartons=new Mailbox<Carton>;
	pBalMessages=new Mailbox<Message>;

	ListeLots lots;
	lots.lots.push_back({"A",2,2,2,1,{100,100,100}});
	lots.lots.push_back({"B",2,2,2,1,{200,200,200}});

	shMemLots.content=&lots;

	gestionnaireLog=new Log(mtxStandardOutput);

	pArgRemplirCarton=new ArgRemplirCarton(pBalPieces,pBalCartons,pBalEvenements
		,pBalMessages,gestionnaireLog,mutCartonPresent
		,&cartonPresent,&mtxLotCourant,&shMemLots,&lotCourant,NULL,&cvThreadRemplirCarton
		,mutCvRemplirCarton,&debutSyncro);

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
	pthread_cancel(threadRemplirCarton);
	sleep(1);
	pthread_exit(NULL);
}