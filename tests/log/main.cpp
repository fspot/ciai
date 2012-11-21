#include <iostream>
#include <stdio.h>
#include "src/log/log.h"
using namespace std;

int main(int argc, char **argv) 
{
    cout << "TEST LOG"<<endl;
    log.Write("test1warning",WARNING);
    log.Write("test2error",ERROR);
    log.Write("test3mzsg",EVENT);
    log.Write("test3STD",ERROR, true);

    return 0;
}
