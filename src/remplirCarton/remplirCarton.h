
/*************************************************************************
                           remplirCarton  -  Tâche qui gère le remplir
*************************************************************************/

//---------- Interface de la tâche remplir carton -------
#if ! defined ( remplirCarton_H )
#define remplirCarton_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "../modeles/modeles.h"
#include "../mailbox/mailbox.h"
#include "../log/log.h" 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct ArgRemplirCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  Mailbox<Message>* pBalMessages;
  Log * gestionnaireLog;
  Mutex* mutCartonPresent;
  sem_t* sem_fin_de_serie;
  bool* pCartonPresent;
  Mutex * lotCourantMutex;
  SharedMemoryLots *shMemLots;
  int * lotCourant;
  sem_t * finDeSerieMutex;
  pthread_cond_t* cv;
  Mutex* mutCv;
  sem_t * debutSyncro;

  ArgRemplirCarton(Mailbox<Piece>* p,Mailbox<Carton>* c,Mailbox<Event>* e,Log * l, Mutex* mcp
    ,sem_t* sfs, bool* cp,Mutex * lcm,SharedMemoryLots * sml,int* lc,sem_t * fsm, pthread_cond_t* cv2
    ,Mutex* mutCv2,sem_t * ds):pBalPieces(p),pBalCartons(c),pBalEvenements(e),gestionnaireLog(l)
    ,mutCartonPresent(mcp),sem_fin_de_serie(sfs),pCartonPresent(cp),lotCourantMutex(lcm),shMemLots(sml)
    ,lotCourant(lc),finDeSerieMutex(fsm),cv(cv2),mutCv(mutCv2),debutSyncro(ds){}
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void * remplirCarton(void * index);


#endif // remplirCarton_H


