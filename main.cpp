#include "Server.hpp"
#include <iostream>

bool	contains_non_digits(std::string str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

void	load_server(Server *serv, char **av)
{
	try
	{
		if (!contains_non_digits(av[1]) || std::stoi(av[1]) > 65535 || std::stoi(av[1]) <= 0)
			throw std::exception();
		serv->set_port(std::stoi(av[1]));
		serv->set_password(std::string(av[2]));
		serv->set_name("IRC.DIAL.L3BAR");
	}
	catch(const std::exception& e)
	{
		std::cerr << "Usage: " << av[0] <<" <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
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