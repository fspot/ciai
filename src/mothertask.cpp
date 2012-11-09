#include<iostream>
#include "modeles/modeles.h"
#include "multithreadObjects/sharedMemory.h"
#include "mailbox/mailbox.h"

using namespace std;

void * remplir_carton_function()
{
  cout<<"remplir crarton"<<endl;
  pthread_exit(0);
}

void * imprimer_function()
{
  cout<<"imprimer "<<endl;
  pthread_exit(0);
} 
void * remplir_palette_function()
{
  cout<<"remplir palette"<<endl;
  pthread_exit(0);
}
void * stocker_palette_function()
{
  cout<<"stocker palette"<<endl;
  pthread_exit(0);
}
void * destocker_palette_function()
{
  cout<<"destocker palette "<<endl;
  pthread_exit(0);
}
void * controleur_function()
{
  cout<<"controleur "<<endl;
  pthread_exit(0);
}
void * serveur_reception_function()
{
  cout<<"serveur reception"<<endl;
  pthread_exit(0);
}
void * serveur_envoi_function()
{
  cout<<"serveur envoi"<<endl;
  pthread_exit(0);
}
void * gestion_series_function()
{
  cout<<"gestion series"<<endl;
  pthread_exit(0);
}

int main()
{


// Initialisation des boites aux lettres
  Mailbox<Event*> balEvenements;
  Mailbox<Carton*> imprimante;
  Mailbox<Carton*> palette;
  Mailbox<Palette*> stockage;

  cout<<"Boites crées"<<endl;
// Initialisation mémoires partagées
  
  SharedMemory infos;
  infos.mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&infos.mutex,NULL);
  cout<<"SHMEM crées"<<endl;
// Mutex

  pthread_mutex_t mutexLog=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutexLog,NULL);
  cout<<"mutex crées"<<endl;
// Creation du gestionnaire de Log
  cout<<"Gestionnaire de Log crée"<<endl;

//Creation des threads
  pthread_t remplir_carton, imprimer, remplir_palette,stocker_palette,destocker_palette,controleur,serveur_reception,serveur_envoi,gestion_series;
  int data1,data2,data3,data4,data5,data6,data7,data8,data9;

  pthread_create (&remplir_carton, NULL, (void *) &remplir_carton_function, NULL);
  pthread_create (&imprimer, NULL, (void *) &imprimer_function, NULL);
  pthread_create (&remplir_palette, NULL, (void *) &remplir_palette_function, NULL);
  pthread_create (&stocker_palette, NULL, (void *) &stocker_palette_function, NULL);
  pthread_create (&destocker_palette, NULL, (void *) destocker_palette_function, NULL);
  pthread_create (&controleur, NULL, (void *) controleur_function, (void *) NULL);
  pthread_create (&serveur_reception, NULL, (void *) serveur_reception_function, NULL);
  pthread_create (&serveur_envoi, NULL, (void *) serveur_envoi_function, (void *) NULL);
  pthread_create (&gestion_series, NULL, (void *) gestion_series_function, (void *) NULL);

  cout<<"Threadcrées"<<endl;
  pthread_join(remplir_carton, NULL);
  pthread_join(imprimer, NULL);
  pthread_join(remplir_palette, NULL);
  pthread_join(stocker_palette, NULL);
  pthread_join(destocker_palette, NULL);
  pthread_join(controleur, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(serveur_envoi, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(gestion_series, NULL);

  cout<<"Thread détruits"<<endl;
  return 0;
  
}
