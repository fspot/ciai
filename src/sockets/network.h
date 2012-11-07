#ifndef NETWORK_H
#define NETWORK_H

void* thread_network(void* arg);

// méthodes d'envoi ou tache d'envoi ?

// void send_carton(carton *c); // envoi au superviseur les infos sur le carton produit
// void send_palette(palette *p); // envoi au superviseur les infos sur la palette produite
// void send_erreur(erreur *e); // envoi au superviseur les infos sur l'erreur survenue
// void send_lot(lot *l); // envoi au superviseur les infos sur le lot terminé

#endif