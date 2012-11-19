/*************************************************************************
                           destock  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de la tâche destock

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <vector>
#include <map>
#include "pthread.h"
#include "../mailbox/mailbox.h"
#include "../modeles/modeles.h"
//------------------------------------------------------ Include personnel
#include "destock.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
using namespace std;
//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

// Méthode d'écriture log
void ecriture_log_destock(Log * unGestionnaire, std::string msg,logType unType)                                                                                     
{
#ifdef DEBUG
  unGestionnaire->Write(msg,unType,true);
#else
  unGestionnaire->Write(msg,unType,false);
#endif 
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void *thread_destock(void * argDestock)
{
  //Conversion des arguments
  ArgDestock *arg = (ArgDestock*) argDestock; 
  ecriture_log_destock(arg->gestionnaireLog,"Lancement de la tâche destock",EVENT);
  ListeCommandes lc;
  while(1) 
    {
      // Attente passive sur lecture
      lc = arg->balCommandes->Pull(); 
      ecriture_log_destock(arg->gestionnaireLog,"Commande recue - destock",EVENT);
      if (lc.fin)
	{
	  // Message de Fin
	  ecriture_log_destock(arg->gestionnaireLog,"Fin de la tâche destock",EVENT);
	  pthread_exit(0);
	  break;
	}
      // Verouillage du stock par la tache de stock
      arg->stock->mutex.lock();

      // vérif commandes OK :
      bool ok = true;
      for (int i=0 ; i<lc.commandes.size() ; i++) {
	string nom = lc.commandes[i].nom;
	int qte = lc.commandes[i].palettes;
	if (arg->stock->stock[nom] < qte) { // oups, pas assez !
	  ok = false;
	}
      }

      // si OK : on destocke
      if (ok) { 
	for (int i=0 ; i<lc.commandes.size() ; i++) {
	  string nom = lc.commandes[i].nom;
	  int qte = lc.commandes[i].palettes;
	  arg->stock->stock[nom] -= qte;
	}
      }

      // Msg réseau acquittement :
      Message msg = {lc.netstr(ok), false};
      arg->balMessages->Push(msg, 2);

      // UNLOCK :
    arg->stock->mutex.unlock();
  }
	
}
