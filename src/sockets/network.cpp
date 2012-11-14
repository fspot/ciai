#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

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

// SIGNATURES

int initListener();
int waitClient(int listener);
int handle(char* data, char *buf);
int handle_BEGIN(char *str);
int handle_MIDDLE(char *str);

// STATICS

static int sockstate;

// CODE STARTS HERE

using namespace std;

int initListener()
{
	// initialisation du main socket (qui reçoit les clients)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        perror("socket()");
        exit(errno);
    }

    Sockaddr_in sin = { 0 };
    sin.sin_addr.s_addr = htonl(INADDR_ANY); /* nous sommes un serveur, nous acceptons n'importe quelle adresse */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    if(bind (sock, (Sockaddr *) &sin, sizeof sin) == -1)
    {
        perror("bind()");
        exit(errno);
    }
    
    if(listen(sock, 5) == -1)
	{
		perror("listen()");
		exit(errno);
	}
	return sock;
}

int waitClient(int listener)
{
	Sockaddr_in csin = { 0 };
	unsigned int sinsize = sizeof(csin);
	int csock = accept(listener, (Sockaddr *)&csin, &sinsize);
	if(csock == -1)
	{
		perror("FAIL ON accept()");
		exit(errno);
	}
	return csock;
}

int handle(char* data, char *buf)
{
	if (MAX_SIZE_MSG-strlen(data) < strlen(buf))
		return -1; // msg > 1024 o

	int ret = 1;
	char *str = strcat(data, buf);
	while (strlen(str) != 0)
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
			break;
	}
	strcpy(data, str); // it works !
	return ret;
}

int handle_BEGIN(char *str)
{
	ListeLots ll;
	int err = parse_I(str, &ll);
	if (err == -1) {
		return BAD_MSG;
	}
	for (int i=0 ; i<ll.lots.size() ; i++) {
		printf("(%d/%d) => Prod %d palettes de %s\n", i+1, ll.tot, ll.lots[i].palettes, ll.lots[i].nom.c_str());
	}
	printf("(BEGIN RCV : %s)\n", str);
	sockstate = MIDDLE;
	return GOOD_MSG;
}

int handle_MIDDLE(char *str)
{
	char c = str[0];
	if (c == REPRISE) {
		int val;
		int err = parse_R(str, &val);
		if (err != -1) {
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
			printf("(%d/%d) => Commande de %d palettes de %s\n", i+1, lc.commandes.size(), lc.commandes[i].palettes, lc.commandes[i].nom.c_str());
		}
		printf("(MIDDLE RCV : %s)\n", str);
		return GOOD_MSG;
	} else {
		puts("KO :(");
		return BAD_MSG;
	}
}

// tâche de réception de messages en provenance du client windows
void* thread_network(void* arg)
{
	puts("Thread network launched");
	NetworkInitInfo *infos = (NetworkInitInfo*) arg;
//	Mailbox<string> *netmb = infos->netmb_ptr;
	int *client = infos->socket_ptr;
	int listener = initListener();
	
	while(1)
	{
		// On va attendre un client...
		sockstate = BEGIN;
		puts("Waiting for *one* client... ... ... ...");
		*client = waitClient(listener);
		puts(">>>> Client got ! :)");
	
		// reception string
		char data[MAX_SIZE_MSG];  // contiendra les msg a traiter (1024 char max!)
		char buffer[32]; // contiendra chaque chunk du stream tcp
		int n = 0; // nombre d'octets lus
		int continuer = 1;
		while(continuer > 0) 
		{
			if((n = recv(*client, buffer, sizeof(buffer)-1, 0)) <= 0)
				break;
			buffer[n] = '\0';
			continuer = handle(data, buffer);
			if (continuer <= 0)
				puts("<<<< Client gone :( ...");
		}
		close(*client);
	}
	
	close(listener);
	
	pthread_exit(0);
}
