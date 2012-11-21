/*************************************************************************
                           parser  -  description
                             -------------------
*************************************************************************/

//---------- Réalisation de l'implémentation du fichier parser

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
//------------------------------------------------------ Include personnel

#include "parser.h"
#include <modeles/modeles.h>


//------------------------------------------------------ Name spaces

using namespace std;



///////////////////////////////////////////////////////////////////  PRIVE

//------------------------------------------------------ Fonctions privées

// Lit une string **alphanumérique** dans str et l'écrit dans s.
// Retour : renvoie la longueur de la string lue. Si 0 => erreur, on renvoie -1.
int parse_nextString(const char* str, string *s)
{
    *s = "";
    int i;
    for(i=0 ; isalnum(str[i]) ; i++) {
	s->push_back(str[i]);
    }
    if (i==0)
	return -1;
    return i;
}

// Lit un **signed int** dans str et l'écrit dans val.
// Lit au maximum 9 digits => min = -999 999 999 ; max = 999 999 999.
// Retour : renvoie le nombre de car lus. Si 0 ou 10 => erreur, on renvoie -1 !
int parse_nextInt(const char* str, int *val)
{
    int i = 0;
    int signe = 1;
    int r = 0;
    if (str[0] == '-') {
	signe = -1;
	i++;
    } else if (str[0] == '+') {
	i++;
    }
    int signe_present = i;
    while(isdigit(str[i]) && i<=MAX_INT+signe_present) {
	int digit = str[i] - '0';
	r *= 10;
	r += digit;
	i++;
    }
    if (i==0 || i==MAX_INT+signe_present+1)
	return -1;
    *val = signe * r;
    return i;
}

// retourne la valeur absolue lue par parse_nextInt
int parse_nextUint(const char* str, int *val)
{
    int err = parse_nextInt(str, val);
    if(err != -1 && *val < 0) {
	*val = (- *val);
    }
    return err;
}

// skip le caractère passé en paramètre dans la chaîne c.
// renvoie le nb de caractères lus (c a d 1). Si erreur, renvoie -1.
int skip(const char* str, char c)
{
    if (str[0] != c)
	return -1;
    return 1;
}

// Lit dans str un I_field, c a d : 
// "nom,palettes,cartons,pieces,rebut,dim[0],dim[1],dim[2]"
// ex I_Field : "#A,10,50,1000,20,33,44,55"
// nom est une string et les autres champs des ints.
// Tout ça est ecrit dans *lot qui ne doit pas être NULL !
// Retour : -1 si erreur ; le nombre de char lus sinon.
int parse_I_nextField(const char* str, Lot *lot)
{
    int err, ofs=0;
	
    if (lot == NULL || !isalnum(str[0]))
	return -1;
	
    err = parse_nextString(str+ofs, &lot->nom); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->palettes); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->cartons); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->pieces); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->rebut); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->dim[0]); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->dim[1]); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &lot->dim[2]); if (err == -1) return -1; else ofs += err;

    return ofs;
}

// Lit dans str un C_field, c a d : 
// "nom,palettes"
// ex I_Field : "#A,10"
// nom est une string palettes est un int.
// Tout ça est ecrit dans *mc qui ne doit pas être NULL !
// Retour : -1 si erreur ; le nombre de char lus sinon.
int parse_C_nextField(const char *str, Commande *mc)
{
    int err, ofs = 0;
    if (mc == NULL || !isalnum(str[0])) {
	return -1;
    }

    err = parse_nextString(str+ofs, &mc->nom); if (err == -1) return -1; else ofs += err;
    err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
    err = parse_nextUint(str+ofs, &mc->palettes); if (err == -1) return -1; else ofs += err;

    return ofs;
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

int parse_I(const char* str, ListeLots *val)
{
    if (val == NULL || str[0] != INITIALISATION)
	return -1;

    int err, i, ofs = 1;
    for (i=0 ; ofs < strlen(str) ; i++)
    {
	Lot lot;
	err = skip(str+ofs, SEP); if (err == -1) return -1; else ofs += err;
	err = parse_I_nextField(str+ofs, &lot); if (err == -1) return -1; else ofs += err;
	val->lots.push_back(lot);
    }
    val->tot = i;
    val->cur = 0;
    return ofs;
}

int parse_C(const char *str, ListeCommandes *val)
{
    if (val == NULL || str[0] != COMMANDE)
	return -1;

    int err, ofs = 1;
    while (ofs < strlen(str))
    {
	Commande c;
	err = skip(str+ofs, SEP); if (err == -1) return -1; else ofs += err;
	err = parse_C_nextField(str+ofs, &c); if (err == -1) return -1; else ofs += err;
	val->commandes.push_back(c);
    }
    return ofs;
}

int parse_R(const char *str, int *val)
{
    if (val == NULL || 
	strlen(str) != 3 || 
	str[0] != REPRISE || 
	str[1] != SEP || 
	!isdigit(str[2]) ||
	str[2]<'0' ||
	str[2]>'3')
	return -1;
	
    *val = str[2]-'0';
    return 0;
}
