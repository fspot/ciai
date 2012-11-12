#include "log.h"



Log::Log(Mutex &mtxStandardOutput):
file(FILENAME, ios::app),
mutexStd(mtxStandardOutput)
{
	if (!file)
	{
		cout << "[WARNING] Log - Could not open file " << FILENAME << "." << endl;
	}
	else
	{
		file << "* * * Ouverture de session - "<< dateStr() << " * * *" << endl;
	}
}


Log::~Log()
{
}

void Log::Write(string text, logType type, bool stdOutput)
{
	string entry;
	if (type = WARNING)
		entry.append("[WARNING] ");
	else if (type = ERROR)
		entry.append("[ERROR] ");

	entry.append(text);

	mutexFile.lock();
	file << dateStr() << entry << endl;
	mutexFile.unlock();

	if (stdOutput)
	{
		mutexStd.lock();
		cout << dateStr() << entry << endl;
		mutexStd.unlock();
	}
}

string Log::dateStr()
{
	now = time(0);
	string str(ctime(&now));
	str.erase(str.end()-1,str.end()); //remove \n inserted by ctime
	return str;
}