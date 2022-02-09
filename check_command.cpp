#include "msg_parse.hpp"
#include "Server.hpp"

void	Server::user_authentication( msg_parse command, User &user)
{
	// std::cout << user.get_pass_check() << std::endl;
	if (command.get_cmd() == "NICK")
	{
		if (__list_nicks.insert(command.get_cmd_params().front()).second)
			user.set_nickname(command.get_cmd_params().front());
		// std::cout << user.get_pass_check() << std::endl;
	}
	else if (command.get_cmd() == "USER")
	{
		user.set_username(command.get_cmd_params().front());
		/*<mode> <unused> <realname>*/
	}
	else if (command.get_cmd() == "PASS")
	{
		if (__password == *command.get_cmd_params().begin())
		{
			std::cout << "PASS CHECKED" << std::endl;
			user.set_pass_check(TRUE);
		}
	}
	std::cout  << std::boolalpha << !user.get_nickname().empty() << std::endl;
	if (!user.get_nickname().empty()  && !user.get_username().empty() && user.get_pass_check())
	{
		write_socket(user.get_fd(), "Welcome to the Internet Relay Network");
	}
}

void	Server::check_command(msg_parse command, User &user)
{
	// std::cout << user.get_pass_check() << std::endl;
	if ((command.get_cmd() == "NICK" || command.get_cmd() == "USER" || command.get_cmd() == "PASS") && !command.get_cmd_params().empty())
	{
		user_authentication(command, user);
		// std::cout << user.get_pass_check() << std::endl;
	}
	//check if command is valid 
}