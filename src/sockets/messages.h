#ifndef MESSAGES_H
#define MESSAGES_H

// les struct InfoInit et InfoCommande sont destinées à être remplacées
// par des objets c++ bien mieux foutus et propres.

typedef struct MsgInit {
	char nom[64];
	int pieces, cartons, palettes, rebut, dim[3];
} MsgInit; 

typedef struct InfoInit {
	MsgInit msgs[64];
	int cur, tot;
} InfoInit;

typedef struct MsgCommande {
	char nom[64];
	int palettes;
} MsgCommande;

typedef struct InfoCommande {
	MsgCommande msgs[64];
	int cur, tot;
} InfoCommande;

#endif