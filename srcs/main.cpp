#include <iostream>

#include "config/Config.h"
#include "Cluster.h"

int main(int ac, char **av)
{
	Config conf;
    if (ac < 2)
    {
        std::cout << "Using default configuration" << std::endl;
		conf.SetFile(DefaultConfig);
    }
	else
	{
		conf.SetFile(av[1]);
	}
	conf.Process();
    Cluster webserv(conf);
    webserv.Run();
}
