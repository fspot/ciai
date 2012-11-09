#include<iostream>
#include "modeles/modeles.h"
#include "multithreadObjects/sharedMemory.h"
#include "mailbox/mailbox.h"

using namespace std;

void * remplir_carton_function()
{
  cout<<"\t\tTâche remplir carton lancée"<<endl;
  pthread_exit(0);
}

void * imprimer_function()
{
  cout<<"\t\tTâche imprimer lancée"<<endl;
  pthread_exit(0);
} 
void * remplir_palette_function()
{
  cout<<"\t\tTâche remplir palette lancée"<<endl;
  pthread_exit(0);
}
void * stocker_palette_function()
{
  cout<<"\t\tTâche stocker palette lancée"<<endl;
  pthread_exit(0);
}
void * destocker_palette_function()
{
  cout<<"\t\tTâche destocker palette lancée"<<endl;
  pthread_exit(0);
}
void * controleur_function()
{
  cout<<"\t\tTâche controleur lancée"<<endl;
  pthread_exit(0);
}
void * serveur_reception_function()
{
  cout<<"\t\tTâche serveur reception lancée"<<endl;
  pthread_exit(0);
}
void * serveur_envoi_function()
{
  cout<<"\t\tTâche serveur envoi lancée"<<endl;
  pthread_exit(0);
}
void * gestion_series_function()
{
  cout<<"\t\tTâche gestion series lancée"<<endl;
  pthread_exit(0);
}

int main()
{
  cout<<"Phase d'initialisation"<<endl;

// Initialisation des boites aux lettres
  Mailbox<Event*> balEvenements;
  Mailbox<Carton*> imprimante;
  Mailbox<Carton*> palette;
  Mailbox<Palette*> stockage;

  cout<<"\tBoites  aux lettre créées"<<endl;
// Initialisation mémoires partagées
  
  SharedMemory infos;
  infos.mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&infos.mutex,NULL);
  cout<<"\tMémoires partagées créeés"<<endl;
// Mutex

  pthread_mutex_t mutexLog=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutexLog,NULL);
  cout<<"\tMutex crées"<<endl;
// Creation du gestionnaire de Log
  cout<<"\tGestionnaire de Log créé"<<endl;

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

  cout<<"\t\t\tTout les thread ont été créés"<<endl;


  cout<<"Phase moteur"<<endl;

  int a;
  cin>>a;

  cout<<endl<<endl<<endl;
  cout<<"Phase de destruction"<<endl;


  pthread_join(gestion_series, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(serveur_envoi, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(controleur, NULL);
  pthread_join(destocker_palette, NULL);
  pthread_join(stocker_palette, NULL);
  pthread_join(remplir_palette, NULL);
  pthread_join(imprimer, NULL);
  pthread_join(remplir_carton, NULL);
  cout<<"\tThread détruits"<<endl;


  cout<<"\tGestionnaire de Log detruit"<<endl;

  cout<<"\tMémoires partagées detruites"<<endl;

  cout<<"\tBoites  aux lettre détruites"<<endl;

  cout<<"\tFin de l'application"<<endl;

  return 0;
  
}
