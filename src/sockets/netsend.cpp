#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread_netsend(void* socket)
{
	int *client = (int*) socket;


	// dummy example :
	for(;;)
	{
		sleep(1);
		printf("<thread 2 says socket = %d>\n", *client);
	}

	/*
	en vérité le thread n'a pas pour seul argument le socket :
	    il a une structure contenant entre autres le socket et la BaL.
	Et en vérité on ne boucle pas à l'infini, on boucle sur les messages de la BaL,
	Puis on envoie ce message (primitive send() TCP)

	NetSendInitInfo *infos = (NetSendInitInfo*) argument; // car argument = (void*)
	BaL<std::string> *bal = infos->bal_ptr;
	int *client = infos->socket_ptr;
	std::string msg;

	while ((msg = bal.get) != EXIT_TASK)
	{
		if(send(*client, msg.c_str(), strlen(msg.c_str()), 0) < 0)
		{
		    cerr << "??" << endl;
		    break;
		}
	}
	*/
	
	pthread_exit(0);
}