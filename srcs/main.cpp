#include <iostream>

#include "Cluster.h"

int main(int ac, char **av)
{
    (void)av;
    if (ac < 2)
    {
        std::cerr << "Using default configuration" << std::endl;
    }
	FakeConfig fk;
    Cluster webserv(fk);
    webserv.Run();
}
