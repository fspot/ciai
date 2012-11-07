#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

// Lit une string **alphanumérique** dans str et l'écrit dans s.
// Retour : renvoie la longueur de la string lue. Si 0 => erreur, on renvoie -1.
int parse_nextString(const char* str, char s[])
{
	int i;
	for(i=0 ; isalnum(str[i]) && i<64 ; i++) {
		s[i] = str[i];
	}
	s[i] = '\0';
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
// ex I_Field : "#A,10,50,1000,20,33,44,55#"
// nom est une string et les autres champs des ints.
// Tout ça est ecrit dans *mi qui ne doit pas être NULL !
// Retour : -1 si erreur ; le nombre de char lus sinon.
int parse_I_nextField(const char* str, MsgInit *mi)
{
	int err, ofs=0;
	
	if (mi == NULL || !isalnum(str[0]))
		return -1;
	
	err = parse_nextString(str+ofs, mi->nom); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->palettes); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->cartons); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->pieces); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->rebut); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->dim[0]); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->dim[1]); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mi->dim[2]); if (err == -1) return -1; else ofs += err;

	return ofs;
}

int parse_I(const char* str, InfoInit *val)
{
	if (val == NULL || str[0] != INITIALISATION)
		return -1;

	int err, i, ofs = 1;
	for (i=0 ; i < MAX_LOT && ofs < strlen(str) ; i++)
	{
		err = skip(str+ofs, SEP); if (err == -1) return -1; else ofs += err;
		err = parse_I_nextField(str+ofs, &val->msgs[i]); if (err == -1) return -1; else ofs += err;
	}
	if (i == MAX_LOT)
		return -1;
	val->tot = i;
	val->cur = 0;
	return ofs;
}

int parse_C_nextField(const char *str, MsgCommande *mc)
{
	int err, ofs = 0;
	if (mc == NULL || !isalnum(str[0])) {
		return -1;
	}

	err = parse_nextString(str+ofs, mc->nom); if (err == -1) return -1; else ofs += err;
	err = skip(str+ofs, SEP2); if (err == -1) return -1; else ofs += err;
	err = parse_nextUint(str+ofs, &mc->palettes); if (err == -1) return -1; else ofs += err;

	return ofs;
}

int parse_C(const char *str, InfoCommande *val)
{
	if (val == NULL || str[0] != COMMANDE)
		return -1;

	int err, i, ofs = 1;
	for (i=0 ; i < MAX_LOT && ofs < strlen(str) ; i++)
	{
		err = skip(str+ofs, SEP); if (err == -1) return -1; else ofs += err;
		err = parse_C_nextField(str+ofs, &val->msgs[i]); if (err == -1) return -1; else ofs += err;
	}
	if (i == MAX_LOT)
		return -1;
	val->tot = i;
	val->cur = 0;
	return ofs;
}

// Parse totalement un message de type R.
// renvoie SHUTDOWN ou REPRENDRE ou PREVOIRPAUSE ou FINPAUSE
int parse_R(const char *str, int *val)
{
	if (val == NULL || strlen(str) != 3 || str[0] != REPRISE || str[1] != SEP || !isdigit(str[2]))
		return -1;
	
	*val = str[2]-'0';
	return 0;
}
