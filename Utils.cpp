#include "Server.hpp"

int		Server::check_for_bad_char(char *nickname)
{
	std::string nick = nickname;
	if (nick.find(",") != std::string::npos || nick.find("&") != std::string::npos || nick.find("#") != std::string::npos)
		return (0);
	for (int i = 0; i < nick.length(); i++)
	{
		if (!isprint(nickname[i]))
			return (0);
	}
	return (1);
}

void	print_command(msg_parse &command)
{
	if (command.get_cmd().size() > 0)
		std::cout << "Command :" << command.get_cmd()  << "|" << std::endl;
	for (std::vector<char *>::iterator it = command.get_cmd_params().begin(); it != command.get_cmd_params().end(); it++)
	{
		std::cout << "param :" << *it << "|" << std::endl;
	}
	if (command.get_additional_param().size() > 0)
		std::cout << "additional param :" << command.get_additional_param() << "|" << std::endl;
}

User		*Server::find_user_in_channel(User &user, Channel &channel)
{
	std::list<User *>::iterator it = channel.get_users().begin();
	for (; it != channel.get_users().end() ; it++)
	{
		if ((*it)->get_nickname() == user.get_nickname())
			return (*it);
	}
	return *it;
}

std::string split_channel_names(std::string &channelName)
{
	int	comma_pos = 0;
	std::string	name;

	if (!channelName.empty() && (comma_pos = channelName.find(',')) != std::string::npos)
	{
		name = channelName.substr(0, comma_pos);
		channelName.erase(0, comma_pos + 1);
		return name;
	}
	else if (comma_pos == std::string::npos && !channelName.empty())
	{
		name = channelName;
		channelName.clear();
		return (name);
	}
	else
		return "";
}