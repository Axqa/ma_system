#include <string>
#include <iostream>
#include "connection.h"
#include <unistd.h>
using namespace std;

int main()
{
    Connection cn("localhost", 6000, 4096);

    cn.sendMessage("(dispinit)");

    usleep(1000000);

    cn.sendMessage("(dispstart)");

    return 0;
}
