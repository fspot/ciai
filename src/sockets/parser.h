#ifndef PARSER_H
#define PARSER_H

#include "messages.h"

#define MAX_INT 9
#define MAX_STR 64
#define MAX_LOT 64

#define SEP '#'
#define SEP2 ','

#define INITIALISATION 'I'
#define REPRISE 'R'
#define SHUTDOWN '0'
#define REPRENDRE '1'
#define PREVOIRPAUSE '2'
#define FINPAUSE '3'
#define COMMANDE 'C'

int parse_I(const char* msg, InfoInit *val);
int parse_R(const char *str, int *val);
int parse_C(const char *str, InfoCommande *val);

#endif