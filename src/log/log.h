#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <ctime>
#include "../multithreadObjects/mutex.h"


#define FILENAME "log.txt"
using namespace std;

typedef enum{EVENT, WARNING, ERROR} logType;


class Log
{
	public:
		Log(Mutex &mtxStandardOutput);
		~Log();

		void Write(string text, logType type, bool stdOutput=false);
		//permet d'ajouter une entree au log, et eventuellement sur la sortie standard.
		//	type:		definit si l'entree est un evenement, un avertisement ou une erreur.
		//	text:		description.
		//	stdOutput:	si true, le message est envoyé sur la sortie standard EN PLUS du log.

	protected:
		ofstream file;
		time_t now;

		string dateStr();
		Mutex mutexFile, &mutexStd;

};

#endif // LOG_H
