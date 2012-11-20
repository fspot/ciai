#include <iostream>
#include <string>

#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#include "netsend.h"
#include "../mailbox/mailbox.h"

using namespace std;


void ecriture_log_netsend(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
  #ifdef DEBUG
	unGestionnaire->Write(msg,unType,true);
  #else
	unGestionnaire->Write(msg,unType,false);
  #endif 
}

void* thread_netsend(void* arg)
{
	NetSendInitInfo *infos = (NetSendInitInfo*) arg;
	ecriture_log_netsend(infos->gestionnaireLog,"Lancement de la tâche serveur envoi",EVENT);
	Mailbox<Message> *netmb = infos->netmb_ptr;
	int *client = infos->socket_ptr;
	string msg;

	/*
	int i=0;
	// dummy example :
	 for(;;)
	 {
		sleep(1);
		cout<<"<thread 2 says socket "<<i<<endl;
		i++;
	 }
	 /**/

	/*
	en vérité le thread n'a pas pour seul argument le socket :
		il a une structure contenant entre autres le socket et la BaL.
	Et en vérité on ne boucle pas à l'infini, on boucle sur les messages de la BaL,
	Puis on envoie ce message (primitive send() TCP)
	*/

	//*
	while (1)
	{
		Message m = netmb->Pull();
		if (m.fin)
			break;

		msg = m.contenu;
		string toSend=msg;
		toSend.resize(toSend.size()-1);
		ecriture_log_netsend(infos->gestionnaireLog,"Le serveur d'envoi vas envoyer le message suivant: "+toSend, EVENT);
		if(send(*client, msg.c_str(), strlen(msg.c_str()), 0) < 0)
		{
			ecriture_log_netsend(infos->gestionnaireLog,"Erreur d'envoi de la tache serveur envoi",ERROR);
			break;
		}
		ecriture_log_netsend(infos->gestionnaireLog,"Message envoyé",EVENT);
	}
	/**/
	ecriture_log_netsend(infos->gestionnaireLog,"Fin de la tache serveur envoi",EVENT);
	pthread_exit(0);
}
