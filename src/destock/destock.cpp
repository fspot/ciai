#include <iostream>
#include <vector>
#include <map>

#include "pthread.h"

#include "stock.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"

using namespace std; 


void *thread_destock(void *argDestock)
{
	ArgDestock arg = (ArgDestock) *argDestock; // cast

	ListeCommandes lc;
	while(1) {
		lc = arg.balCommandes->Pull(); // bloquant
		if (lc.fin)
			break;

		// LOCK :
		arg.stock->mutex.lock();

		// vérif commandes OK :
		bool ok = true;
		for (int i=0 ; i<lc.commandes.size() ; i++) {
			string nom = lc.commandes[i].nom;
			int qte = lc.commandes[i].palettes;
			if (arg.stock->stock[nom] < qte) { // oups, pas assez !
				ok = false;
			}
		}

		// si OK : on destocke
		if (ok) { 
			for (int i=0 ; i<lc.commandes.size() ; i++) {
				string nom = lc.commandes[i].nom;
				int qte = lc.commandes[i].palettes;
				arg.stock->stock[nom] -= qte;
			}	
		} else { // si PAS OK : msg réseau ack + remonter dans balEvent ?

		}

		// UNLOCK :
		arg.stock->mutex.unlock();
	}

	pthread_exit(0);
}