#include<iostream>
#include "modeles/modeles.h"
#include "multithreadObjects/sharedMemory.h"
#include "mailbox/mailbox.h"


void * remplir_carton()
{
}

void * imprimer()
{
} 
void * remplir_palette()
{
}
void * stocker_palette()
{
}
void * destocker_palette()
{
}
void * controleur()
{
}
void * serveur_reception()
{
}
void * serveur_envoi()
{
}
void * gestion_series()
{
}

int main()
{


// Initialisation des boites aux lettres
  Mailbox<Error*> balEvenements;
  Mailbox<Carton*> imprimante;
  Mailbox<Carton*> palette;
  Mailbox<Palette*> stockage;


// Initialisation mémoires partagées
  
  sharedMemory infos;
  infos.mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&infos.mutex,NULL);

// Mutex

  pthread_mutex_t mutexLog=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutexLog,NULL);

// Creation du gestionnaire de Log


//Creation des threads
  pthread_t remplir_carton, imprimer, remplir_palette,stocker_palette,destocker_palette,controleur,serveur_reception,serveur_envoi,gestion_series;
  int data1,data2,data3,data4,data5,data6,data7,data8,data9;

  pthread_create (&remplir_carton, NULL, (void *) &remplir_carton_function, (void *) &data1);
  pthread_create (&imprimer, NULL, (void *) &imprimer_function, (void *) &data2);
  pthread_create (&remplir_palette, NULL, (void *) &remplir_palette_function, (void *) &data3);
  pthread_create (&stocker_palette, NULL, (void *) &stocker_palette_function, (void *) &data4);
  pthread_create (&destocker_palette, NULL, (void *) &destocker_palette_function, (void *) &data5);
  pthread_create (&controleur, NULL, (void *) &controleur_function, (void *) &data6);
  pthread_create (&serveur_reception, NULL, (void *) &serveur_reception_function, (void *) &data7);
  pthread_create (&serveur_envoi, NULL, (void *) &serveur_envoi_function, (void *) &data8);
  pthread_create (&remplir_carton, NULL, (void *) &remplir_carton_function, (void *) &data9);

  pthread_join(remplir_carton, NULL);
  pthread_join(imprimer, NULL);
  pthread_join(remplir_palette, NULL);
  pthread_join(stocker_palette, NULL);
  pthread_join(destocker_palette, NULL);
  pthread_join(controleur, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(serveur_envoi, NULL);
  pthread_join(serveur_reception, NULL);
  pthread_join(remplir_carton, NULL);


  return 0;
  
}
