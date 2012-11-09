#ifndef MODELES_H
#define MODELES_H

#include <vector>
#include <string>
#include <sstream>

// http://lite.framapad.org/p/ideemodeleciai

struct Lot {
	std::string nom;
	int pieces, cartons, palettes, rebut, dim[3];
	std::string str() {
		std::stringstream ss;
		ss << "Lot " << nom << " " << palettes;
		return ss.str();
	}
}; 


enum EventType            
{
    abscenceCarton,      
    panneImprimante ,     
    absencePalette,         
    tauxErreurTE,
    fileAttentePlein,      
    arretUrgence,
    repriseErreur,
    pause,
    erreurEmbalage,
    erreurCommande,
    reprisePause		
} ;


struct Event {
	EventType event	;
}; 

struct ListeLots {
	std::vector<Lot> lots;
	int cur, tot;
};

struct Commande {
	std::string nom;
	int palettes;
};

struct ListeCommandes {
	std::vector<Commande> commandes;
};

struct Erreur {
	int code;
};

struct Palette {
	int id;
	Lot *lot;
};

struct Carton {
	int id;
	Palette *palette;
};

struct Piece {
	Carton *carton;
	int dim[3];
};

#endif
