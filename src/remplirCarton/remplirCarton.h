
/*************************************************************************
                           remplirCarton  -  Tâche qui gère le remplir
*************************************************************************/

//---------- Interface de la tâche remplir carton -------
#if ! defined ( remplirCarton_H )
#define remplirCarton_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <modeles/modeles.h>
#include <mailbox/mailbox.h>
#include <log/log.h> 

//------------------------------------------------------------- Constantes 

//------------------------------------------------------------------ Types 
struct ArgRemplirCarton{
  Mailbox<Piece>* pBalPieces;
  Mailbox<Carton>* pBalCartons;
  Mailbox<Event>* pBalEvenements;
  Mailbox<Message>* pBalMessages;
  Log * gestionnaireLog;
  Mutex* mutCartonPresent;
  bool* pCartonPresent;
  Mutex * lotCourantMutex;
  SharedMemoryLots *shMemLots;
  int * lotCourant;
  pthread_cond_t* cv;
  pthread_mutex_t* mutCv;
  sem_t * debutSyncro;

  ArgRemplirCarton() {}
  ArgRemplirCarton(Mailbox<Piece>* p,Mailbox<Carton>* c,Mailbox<Event>* e,Mailbox<Message>* m,Log * l, Mutex* mcp
   , bool* cp,Mutex * lcm,SharedMemoryLots * sml,int* lc,sem_t * fsm, pthread_cond_t* cv2
    ,pthread_mutex_t* mutCv2,sem_t * ds):pBalPieces(p),pBalCartons(c),pBalEvenements(e),pBalMessages(m),gestionnaireLog(l)
    ,mutCartonPresent(mcp),pCartonPresent(cp),lotCourantMutex(lcm),shMemLots(sml)
    ,lotCourant(lc),cv(cv2),mutCv(mutCv2),debutSyncro(ds){}
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void * remplirCarton(void * index);


#endif // remplirCarton_H


