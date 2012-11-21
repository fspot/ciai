/*************************************************************************
  piece  -  Tâche qui gère la génération des pièces.
*************************************************************************/

//---------- Interface de la tâche piece ---

#ifndef PIECE_H
#define PIECE_H

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

#include <modeles/modeles.h>
#include <mailbox/mailbox.h>
#include <mutex/mutex.h>
#include <log/log.h>

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 

struct ArgPiece
{
    Mailbox<Piece>* balPiece;
    Mailbox<Event>* balEvenements;
    Log * gestionnaireLog;
    sem_t * debutSyncro;
    Mutex *clapet;
    int * lotCourant;
    Mutex * lotCourantMutex;
    SharedMemoryLots * shMemLots;
};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void *thread_piece(void *argPiece);

#endif
