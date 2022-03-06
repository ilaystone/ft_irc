#include "Server.hpp"
#include "msg_parse.hpp"


int		Server::AWAY_handler(msg_parse &command, User &user)
{
	if (command.get_additional_param().size())
	{
		user.set_modes('a');
		if (!command.get_additional_param().empty())
		{
			user.set_away_msg(command.get_additional_param());
			std::string	full_msg = ":" + this->__name + " 306 AWAY :You are now marked as being away\n"; 
			send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
		}
		else
			write_reply(user, RPL_NOWAWAY, command);
	}
	else
	{
		user.unset_modes('a');
		user.get_away_msg().clear();
		write_reply(user, RPL_UNAWAY, command);
	}
	return (1);
}

void	Server::remove_user(User &user)
{
	for (std::list<User>::iterator it = __users.begin(); it != __users.end(); it++)
	{
		if (*it == user)
		{
			__users.erase(it);
			(*it).unset_fd();
		}
	}
}

void	Server::QUIT_handler(User &user, msg_parse &command)
{
	std::string full_msg = "Closing Link: HOST_NAME";

	std::cerr << command.get_additional_param().size() << std::endl;
	if (command.get_additional_param().size() > 0)
		full_msg = ":" + this->__name + " QUIT :" + command.get_additional_param() + "\n";
	if (user.get_channels().size())
		part_from_all_channels(user);
	write_socket(user.get_fd(), full_msg);
	remove_user(user);
	__list_nicks.erase(user.get_nickname());
}

void	Server::QUIT_server(User &user)
{
	std::string full_msg = "Closing Link: HOST_NAME";

	if (user.get_channels().size())
		part_from_all_channels(user);
	write_socket(user.get_fd(), full_msg);
	remove_user(user);
	__list_nicks.erase(user.get_nickname());
}
