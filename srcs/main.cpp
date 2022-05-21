#include <iostream>

#include "config/Config.h"
#include "Cluster.h"

int main(int ac, char **av)
{
	std::string file_path;
    if (ac < 2)
    {
        std::cout << "Using default configuration" << std::endl;
		file_path = config::DefaultConfig;
    }
	else
	{
		file_path = av[1];
	}
	config::Config conf(file_path);
	conf.Process();
    Cluster webserv(conf);
    webserv.Run();
}
