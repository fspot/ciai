#ifndef MODELES_H
#define MODELES_H

#include <vector>
#include <string>
#include <sstream>
#include "../mailbox/mailbox.h"

// http://lite.framapad.org/p/ideemodeleciai

/* Note de Fred :
   la méthode netstr() renvoie une chaîne formatée de façon à ce qu'elle puisse être envoyée sur le réseau.
   Du coup si vous voulez modifier les structures suivantes, pensez simplement à faire en sorte que ces
   méthodes fonctionnent toujours (en y ajoutant des paramètres si besoin).
*/

struct Lot {
  std::string nom;
  int pieces, cartons, palettes, rebut, dim[3];
  std::string netstr() { return "L\r\n"; } // msg envoyé lors du passage au lot suivant
}; 


enum Task
  {
    REMPLIRCARTON,
    REMPLIRPALETTE,
    IMPRIMER,
    STOCKERPALETTE,
    DESTOCKERPALETTE
  };

enum EventType            
  {
    ABSCARTON,      
    PANNEIMPRIM ,     
    ABSPALETTE,         
    TAUXERR,
    FILEATTPLEINE,      
    ARTURG,
    REPRISEERREUR,
    PAUSE,
    ERREMBALAGES,
    ERRCOMMANDE,
    REPRISEPAUSE,
    FIN
  } ;


struct Event {
  EventType event	;
  Event(EventType e):event(e){}
}; 




struct Message {
  std::string contenu;
}; 

struct ListeLots {
  std::vector<Lot> lots;
  int cur, tot;
};

struct Commande {
  std::string nom; // nom du produit
  int palettes; // nombre de palettes commandées
};

struct ListeCommandes {
	std::vector<Commande> commandes;
	std::string netstr(bool ok) {
		if (ok)
			return "A#1\r\n"; // commandes ok (disponibilité ok)
		else
			return "A#0\r\n"; // pas ok
	}
};

struct Palette {
	int id;
	Lot *lot;
	std::string netstr() {
		std::stringstream ss;
		ss << "P#" << lot->nom << "," << lot->cartons << "\r\n"; // msg envoyé qd palette finie.
		return ss.str();
	}
};

struct Carton {
	int id;
	int nbrebut;
	std::string netstr() {
		std::stringstream ss;
		ss << "B#" << lot->nom << "," << lot->pieces << "," << nbrebut << "\r\n"; // msg envoyé qd carton fini.
		return ss.str();
	}
	// Palette *palette; // ?
};

struct Piece {
	// Carton *carton; // ?
	int dim[3]; // en mm
};

typedef struct tInitRemplissageCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  pthread_mutex_t* mutCartonPresent;
  sem_t* sem_fin_de_serie;
  bool* pCartonPresent;
  Lot* lots;
  unsigned int nbLots;
  pthread_cond_t* cv;
  pthread_mutex_t mutCv;
}tInitRemplissageCarton;

#endif
