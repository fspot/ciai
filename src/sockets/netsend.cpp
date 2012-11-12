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

void* thread_netsend(void* arg)
{
	NetSendInitInfo *infos = (NetSendInitInfo*) arg;
	Mailbox<Message> *netmb = infos->netmb_ptr;
	int *client = infos->socket_ptr;
	string msg;
	int i=0;
	// dummy example :
	 for(;;)
	 {
	 	sleep(1);
	 	cout<<"<thread 2 says socket "<<i<<endl;
		i++;
	 }

	/*
	en vérité le thread n'a pas pour seul argument le socket :
	    il a une structure contenant entre autres le socket et la BaL.
	Et en vérité on ne boucle pas à l'infini, on boucle sur les messages de la BaL,
	Puis on envoie ce message (primitive send() TCP)
	*/
	/*


	while ((msg = netmb->Pull().contenu) != "EXIT_TASK")
	{
		if(send(*client, msg.c_str(), strlen(msg.c_str()), 0) < 0)
		{
		    cerr << "send() error" << endl;
		    break;
		}
		cout << "<thread 2 sent \"" << msg << "\">" << endl;
	}
	*/
	pthread_exit(0);
}
