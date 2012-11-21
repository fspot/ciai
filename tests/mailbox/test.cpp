#include <iostream>
#include <string>

#include "mailbox.h"

using namespace std;

int main()
{
    // création
    Mailbox<string> mb;

    // envoi
    mb.Push("ça", 2);
    mb.Push("va", 2);
    mb.Push(" ?", 3);
    mb.Push("Salut, ", 1);

    // réception
    for(;;) {
	string rcv = mb.Pull();
	cout << rcv << endl;
    }

    return 0;
}
