/*************************************************************************
                           log -  description
                             -------------------
*************************************************************************/

//---------- Interface de la classe Log ------

#ifndef LOG_H
#define LOG_H

//--------------------------------------------------- Interfaces utilisées
#include <fstream>
#include <iostream>
#include <ctime>
#include <mutex/mutex.h>
//------------------------------------------------------------- Constantes 
#define FILENAME "log.txt"
using namespace std;
//------------------------------------------------------------------ Types 
typedef enum{EVENT, WARNING, ERROR} logType;
//------------------------------------------------------------------------ 
// Rôle de la classe Log
//
//	Permet de gérer l'ecriture des logs
//------------------------------------------------------------------------ 

class Log
{
//----------------------------------------------------------------- PUBLIC

public:
//----------------------------------------------------- Méthodes publiques
public:
    Log(Mutex &mtxStandardOutput);
    ~Log();

    void Write(string text, logType type, bool stdOutput=false);
    //permet d'ajouter une entree au log, et eventuellement sur la sortie standard.
    //	type:		definit si l'entree est un evenement, un avertisement ou une erreur.
    //	text:		description.
    //	stdOutput:	si true, le message est envoyé sur la sortie standard EN PLUS du log.



//------------------------------------------------------------------ PRIVE 

protected:
    ofstream file;
    time_t now;
    string dateStr();
    Mutex mutexFile, &mutexStd;

};

#endif // LOG_H
