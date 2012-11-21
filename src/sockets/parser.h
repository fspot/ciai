#ifndef PARSER_H
#define PARSER_H

#include "../modeles/modeles.h"

#define MAX_INT 9 // taille maximale d'un int (en nombre de caracteres)

// séparateurs entre champs des messages :
#define SEP '#'
#define SEP2 ','

#define INITIALISATION 'I'

#define REPRISE 'R'
#define SHUTDOWN '0'
#define REPRENDRE '1'
#define PREVOIRPAUSE '2'
#define FINPAUSE '3'

#define COMMANDE 'C'

// parse un message de type I (initialisation)
int parse_I(const char* msg, ListeLots *val);

// parse un message de type R (reprise / pause / arret)
// Renvoie SHUTDOWN ou REPRENDRE ou PREVOIRPAUSE ou FINPAUSE
int parse_R(const char *str, int *val);

// parse un message de type C (liste de commandes).
int parse_C(const char *str, ListeCommandes *val);

#endif