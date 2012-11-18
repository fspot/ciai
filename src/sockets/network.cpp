#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#include "network.h"
#include "parser.h"
#include "../modeles/modeles.h"

#include <vector>
#include <string>

// DEFINES

#define PORT 8761
#define MAX_SIZE_MSG 1024
#define GOOD_MSG 1
#define BAD_MSG -1

// TYPES

typedef struct sockaddr_in Sockaddr_in;
typedef struct sockaddr Sockaddr;
enum e_sockstate {
	BEGIN,
	MIDDLE
};

// STATICS

static int sockstate;
static SharedMemoryLots *stat_lots; // mem partagée avec toutes les infos sur les lots
static sem_t *stat_sync; // sémaphore de synchronisation de début d'appli
static NetworkInitInfo * infos;

// SIGNATURES

int initListener();
int waitClient(int listener);
int handle(char* data, char *buf);
int handle_BEGIN(char *str);
int handle_MIDDLE(char *str);

// CODE STARTS HERE

using namespace std;

void ecriture_log_network(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
    unGestionnaire->Write(msg,unType,true);
  #else
    unGestionnaire->Write(msg,unType,false);
  #endif 
}


int initListener(NetworkInitInfo *infos)
{
	// initialisation du main socket (qui reçoit les clients)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        ecriture_log_network(infos->gestionnaireLog,"Erreur socket - serveur reception",ERROR);
        exit(errno);
    }

    Sockaddr_in sin = { 0 };
    sin.sin_addr.s_addr = htonl(INADDR_ANY); /* nous sommes un serveur, nous acceptons n'importe quelle adresse */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);

    if(bind (sock, (Sockaddr *) &sin, sizeof sin) == -1)
    {
		ecriture_log_network(infos->gestionnaireLog,"Erreur bind - serveur reception",ERROR);
		exit(errno);
    }
    
    if(listen(sock, 5) == -1)
	{
		ecriture_log_network(infos->gestionnaireLog,"Erreur ecoute - serveur reception",ERROR);
		exit(errno);
	}
	return sock;
}

int waitClient(int listener, NetworkInitInfo *infos)
{
	Sockaddr_in csin = { 0 };
	unsigned int sinsize = sizeof(csin);
	int csock = accept(listener, (Sockaddr *)&csin, &sinsize);
	if(csock == -1)
	{
                ecriture_log_network(infos->gestionnaireLog,"Erreur connexion - serveur reception",ERROR);
		exit(errno);
	}
	return csock;
}

int handle(char* data, char *buf)
{
	if (MAX_SIZE_MSG-strlen(data) < strlen(buf))
		return -1; // msg > 1024 o

	int ret = 1;
	bool flagEndWhile=true;
	char *str = strcat(data, buf);
	while ((strlen(str) != 0) && flagEndWhile)
	{
		int ofs = 0; // ofs --> offset
		while(strlen(str)-ofs >= 2 && (str[ofs+1] != '\r' && str[ofs+2] != '\n'))
			ofs++;

		if (strlen(str)-ofs < 2) break; // str est un msg incomplet
		// sinon str pointe sur un début de message complet, 
		// et str[ofs+3] est le début de la suite ('\0' ou non).
		char message[MAX_SIZE_MSG];
		strncpy(message, str, ofs+1);
		message[ofs+1] = '\0';
		str += (ofs+3); // str pointe plus loin dans data
		switch(sockstate)
		{
			case BEGIN:
				ret = handle_BEGIN(message);
				break;
			case MIDDLE:
				ret = handle_MIDDLE(message);
				break;
		}
		if (ret <= 0)
		{
			flagEndWhile=false;
		}

	}
	strcpy(data, str); // it works !
	return ret;
}

int handle_BEGIN(char *str)
{
	int err = parse_I(str, stat_lots->content);
	if (err == -1) {
		return BAD_MSG;
	}
	for (int i=0 ; i < stat_lots->content->lots.size() ; i++) {
		/*printf("(%d/%d) => Prod %d palettes de %s\n",
			i+1, 
			stat_lots->content->tot, 
			stat_lots->content->lots[i].palettes, 
			stat_lots->content->lots[i].nom.c_str()
		);*/
	}
	//printf("(BEGIN RCV : %s)\n", str);
	sockstate = MIDDLE;
	for(int i=0;i<2;i++)
	{
		sem_post(stat_sync);
	}		
	return GOOD_MSG;
}

int handle_MIDDLE(char *str)
{
	char c = str[0];
	if (c == REPRISE) {
		int val;
		int err = parse_R(str, &val);
		if (err != -1) 
		{
			switch (val)
			{
				case 0:
					infos->balEvenements->Push(Event(FINERREUR),0);
					cout<<"Remonte"<<endl;
				break;
				case 1:
					infos->balEvenements->Push(Event(REPRISEERREUR),0);
				break;
				case 2:
					infos->balEvenements->Push(Event(PAUSE),0);
				break;
				case 3:
					infos->balEvenements->Push(Event(REPRISEPAUSE),0);
				break;
				default:
				break;
			};
			printf("(MIDDLE RCV : %s, VAL IS %d)\n", str, val);
			return GOOD_MSG;
		}
		return BAD_MSG;
	} else if (c == COMMANDE) {
		ListeCommandes lc;
		int err = parse_C(str, &lc);
		puts("A");
		if (err == -1) {
			return BAD_MSG;
		}
		for (int i=0 ; i<lc.commandes.size() ; i++) {
			//printf("(%d/%d) => Commande de %d palettes de %s\n", i+1, lc.commandes.size(), lc.commandes[i].palettes, lc.commandes[i].nom.c_str());
		}
		//printf("(MIDDLE RCV : %s)\n", str);
		return GOOD_MSG;
	} else {
		//puts("KO :(");
		return BAD_MSG;
	}
}

// tâche de réception de messages en provenance du client windows
void* thread_network(void* arg)
{
	infos = (NetworkInitInfo*) arg;
        ecriture_log_network(infos->gestionnaireLog,"Lancement de la tâche serveur reception",EVENT);
	// Mailbox<string> *netmb = infos->netmb_ptr;
	int *client = infos->socket_ptr;
	stat_lots = (SharedMemoryLots*) infos->shMemLots;
	stat_sync = infos->debutSyncro;
	int listener = initListener(infos);
	while(1)
	{
		// On va attendre un client...
		sockstate = BEGIN;
                ecriture_log_network(infos->gestionnaireLog,"Attente connexion - serveur reception",EVENT);
		*client = waitClient(listener,infos);
                ecriture_log_network(infos->gestionnaireLog,"Client connecté - serveur reception",EVENT);
	
		// reception string
		char data[MAX_SIZE_MSG];  // contiendra les msg a traiter (1024 char max!)
		char buffer[32]; // contiendra chaque chunk du stream tcp
		int n = 0; // nombre d'octets lus
		int continuer = 1;
		while(continuer > 0) 
		{
			if((n = recv(*client, buffer, sizeof(buffer)-1, 0)) <= 0)
			{
				close(listener);
                		ecriture_log_network(infos->gestionnaireLog,"Client deconnécté && Fin de la tache serveur reception",EVENT);
				pthread_exit(0);
			}
			buffer[n] = '\0';
			continuer = handle(data, buffer);

			if (continuer <= 0)
                		ecriture_log_network(infos->gestionnaireLog,"Client deconnécté - serveur reception",EVENT);
		}
		close(*client);
	}
	

}
