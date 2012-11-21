/*************************************************************************
                           modeles.h  -  description
                             -------------------
*************************************************************************/

//---------- Interface du fichier modeles
#if ! defined ( modeles_H )
#define modeles_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
// includes C++
#include <map>
#include <vector>
#include <string>
#include <sstream>

// includes C
#include <pthread.h>

// includes personnels
#include <mailbox/mailbox.h>
//------------------------------------------------------------- Types

// Structure qui definit un lot
struct Lot 
{
    std::string nom;
    int pieces, cartons, palettes, rebut, dim[3];
    std::string netstr() { return "L#\r\n"; } // msg envoyé lors du passage au lot suivant (== nouveau lot)
}; 

// La liste des tache de production
enum Task
{
    REMPLIRCARTON,
    REMPLIRPALETTE,
    IMPRIMER,
    STOCKERPALETTE,
    DESTOCKERPALETTE
};


// La liste des évènements gérrés par la tache controleur
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
    ABSPIECE,
    FIN,
    FINERREUR,
    FINLAST,
    FINSERIE
} ;

// Une structure evenement
struct Event {
    EventType event	;
    Event(EventType e):event(e){}
    std::string netstr() {
	std::stringstream ss;
	ss << "E#" << event << "\r\n"; // msg envoyé qd palette finie.
	return ss.str();
    }
}; 

// Un message gérré par la tache d'envoi réseau
struct Message {
    std::string contenu;
    bool fin; // vaut true si fin de la tâche, false sinon.
}; 


// L'ensemble des lots à produire
struct ListeLots {
    std::vector<Lot> lots;
    int tot;
    int cur;
};


// Une commande à effectuer par la tache destock
struct Commande {
    std::string nom; // nom du produit
    int palettes; // nombre de palettes commandées
};


// L'ensemble des commandes a effectuer
struct ListeCommandes
{
    std::vector<Commande> commandes;
    std::string netstr(bool ok)
	{
	    if (ok)
		return "A#1\r\n"; // commandes ok (disponibilité ok)
	    else
		return "A#0\r\n"; // pas ok
	}
    bool fin; // vaut true si fin de la tâche, false sinon.
};


// La structure d'une palette
struct Palette
{
    int id;
    Lot *lot;
    std::string netstr() 
	{
	    std::stringstream ss;
	    ss << "P#" << lot->nom << "\r\n"; // msg envoyé qd palette finie.
	    return ss.str();
	}
    bool fin; // vaut true si fin de la tâche, false sinon.
};

//La structure d'un carton
struct Carton {
    int id;
    Lot *lot;
    int nbrebut;
    std::string netstr_rempli() 
	{
	    std::stringstream ss;
	    ss << "B#" << lot->nom << "," << nbrebut << "\r\n"; // msg envoyé qd carton rempli.
	    return ss.str();
	}
    std::string netstr_palette() 
	{
	    std::stringstream ss;
	    ss << "D#" << lot->nom << "\r\n"; // msg envoyé qd carton paletté.
	    return ss.str();
	}
    bool fin; // vaut true si fin de la tâche, false sinon.
};


// La structure d'une piece
struct Piece {
    int dim[3]; // en mm
    bool fin; // vaut true si fin de la tâche, false sinon.
};


// SHARED MEMORY :

// Mémoire partagée qui regrouppe l'enmeble des lots
struct SharedMemoryLots
{
    Mutex mutex;
    ListeLots * content;	
};


// Mémoire partagée qui gère l'ensemble des stock
struct SharedMemoryStock // utilisé par stock et destock :
{
    Mutex mutex;
    std::map<std::string, int> stock; // associe un nom de lot à un compteur (nb de palettes)
};


#endif
