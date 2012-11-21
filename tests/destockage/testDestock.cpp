
#include <pthread.h>
#include <iostream>
#include <string>
#include <signal.h>
#include "../../src/modeles/modeles.h"
#include "../../src/mailbox/mailbox.h"
#include "../../src/destock/destock.h"
#include "../../src/log/log.h"

using namespace std;

// creation du Log
static Mutex mtxStandardOutput;
static Log gestionnaireLog(mtxStandardOutput);

//Fontions pour tests
void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType);

int main() {

    pthread_cond_t condDP=PTHREAD_COND_INITIALIZER;
    pthread_mutex_t condDPM=PTHREAD_MUTEX_INITIALIZER;
	
    // initialisation de attr et param pour les threads temps réel :
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sched_param param;
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_getschedparam(&attr, &param);

    // Initialisation des boites aux lettres
    Mailbox<Event>  balEvenements;
    Mailbox<ListeCommandes> balCommandes;
    Mailbox<Message> balMessages;

    // * shmem du stock :
    SharedMemoryStock stock;

    //Creation du thread stocker_palette
    pthread_t destocker_palette;

    //Création du thread destocker palette
    ArgDestock argDestock;
    argDestock.gestionnaireLog=&gestionnaireLog;
    argDestock.balEvenements = &balEvenements;
    argDestock.balCommandes = &balCommandes;
    argDestock.cv = &condDP;
    argDestock.mutCv = &condDPM;
    argDestock.stock = &stock;
    argDestock.balMessages = &balMessages;
	
    //placement des commandes dans la bal
    int nbListCommandes=2;
    int nbCommandes=3;
    for(int i =0;i<nbListCommandes;i++)
    {
	ListeCommandes listeCommandes;
	vector<Commande> commandes;
	for(int j =0;j<nbCommandes;j++)
	{
	    Commande commande;
	    commande.palettes=5;	
	    commande.nom="Commande_test";		
	    commandes.push_back(commande);
	}
	listeCommandes.commandes=commandes;
	if(i==nbListCommandes-1)
	{
	    listeCommandes.fin=true;
	}
	else
	{
	    listeCommandes.fin=false;
	}
	balCommandes.Push(listeCommandes);
    }
    sleep(1);
	
    cout<<"5"<<endl;
    // thread temps partagé ou prio 70 ?
    pthread_create (&destocker_palette, NULL, thread_destock, (void*) &argDestock);

    cout<<"6"<<endl;

    pthread_join(destocker_palette, NULL);

    ecriture_log_mere(&gestionnaireLog,"Fin de la tâche destocker_palette",EVENT);

    return 0;

}

void ecriture_log_mere(Log * unGestionnaire, std::string msg,logType unType)
{
#ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
#else
    unGestionnaire->Write(msg,unType,false);
#endif
}
