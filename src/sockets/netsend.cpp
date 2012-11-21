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

	while (1)
	{
		Message m = netmb->Pull(); // on reçoit un message
		if (m.fin)
			break;

		msg = m.contenu;
		string toLog=msg;
		toLog.resize(toLog.size()-1);
		ecriture_log_netsend(infos->gestionnaireLog,"Le serveur d'envoi vas envoyer le message suivant: "+toLog, EVENT);
		if(send(*client, msg.c_str(), strlen(msg.c_str()), 0) < 0) // envoie TCP
		{
			ecriture_log_netsend(infos->gestionnaireLog,"Erreur d'envoi de la tache serveur envoi",ERROR);
			break;
		}
		ecriture_log_netsend(infos->gestionnaireLog,"Message envoyé",EVENT);
	}
	
	ecriture_log_netsend(infos->gestionnaireLog,"Fin de la tache serveur envoi",EVENT);
	pthread_exit(0);
}
