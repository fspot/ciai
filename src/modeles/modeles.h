#ifndef MODELES_H
#define MODELES_H

#include <vector>
#include <string>
#include <sstream>

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

struct Erreur {
	int code;
	std::string netstr() {
		std::stringstream ss;
		ss << "E#" << code << "\r\n"; // msg envoyé lors d'une erreur.
		return ss.str();
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
	Lot *lot;
	std::string netstr() {
		std::stringstream ss;
		ss << "B#" << lot->nom << "," << lot->pieces << "," << nbrebut << "\r\n"; // msg envoyé qd carton fini.
		return ss.str();
	}
	// Palette *palette; // ?
};

struct Piece {
	// Carton *carton; // ?
	int x,y,z; // en mm
};

#endif