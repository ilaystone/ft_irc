#include "Server.hpp"
#include <iostream>

void	load_server(Server *serv, char **av)
{
	serv->set_port(std::atoi(av[1]));
	serv->set_password(std::string(av[2]));
	serv->set_name("IRC_HOST");
}

int		main(int ac, char **av)
{
	Server	serv(0000, "pass");

	if (ac == 3)
	{
		load_server(&serv, av);
		std::cout << "Server Init \n";
		serv.init_server();
		std::cout << "Server Started" << std::endl;
		serv.start_server();
	}
	else
	{
		std::cerr << "Usage: " << av[0] <<" <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
}