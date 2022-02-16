#include "msg_parse.hpp"
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
	std::cout << "Command :" << command.get_cmd()  << "|" << std::endl;
	for (std::vector<char *>::iterator it = command.get_cmd_params().begin(); it != command.get_cmd_params().end(); it++)
	{
		std::cout << "param :" << *it << "|" << std::endl;
	}
	std::cout << "additional param :" << command.get_additional_param() << "|" << std::endl;
}

User		*Server::find_user_in_channel(User user, Channel &channel)
{
	std::list<User *>::iterator it = channel.get_users().begin();
	for (; it != channel.get_users().end() ; it++)
	{
		if ((*it)->get_nickname() == user.get_nickname())
			return (*it);
	}
	return *it;
}

void	Server::WHOIS_handler(msg_parse &command, User &user)
{
	User	*tmp;
	if (command.get_cmd_params().size() == 0)
		write_reply(user, ERR_NONICKNAMEGIVEN, command);
	else
		tmp = getuserbynick(command.get_cmd_params().front());
	if (tmp == nullptr)
		write_reply(user, ERR_NOSUCHNICK, command);
	else if (command.get_cmd_params().size() == 1 && tmp)
	{
		write_reply(user, RPL_WHOISUSER, command);
		write_reply(user, RPL_WHOISSERVER, command);
		write_reply(user, RPL_ENDOFWHOIS, command);
	}
}

int		Server::PRIVMSG_handler(msg_parse &command, User &user)
{
	User	*receiver;
	std::string	tmp;
	std::string	target;
	std::string	channel_name;
	std::list<Channel>::iterator ch_it;
	std::string mail = user.full_id() + " ";
	char	msg[command.get_additional_param().length() + mail.length() + 1];

	// number of params is 1 + text to send
	// check if param[1] is a user or channel
	if (command.get_additional_param().empty())
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NOTEXTTOSEND, command) : 0;
	else
	{
		strcpy(msg, mail.c_str());
		strcpy(msg + mail.length(), command.get_additional_param().c_str());
	}
	if (command.get_cmd_params().size() < 1)
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NORECIPIENT, command) : 0;
	target = command.get_cmd_params()[0];
	if (command.get_cmd_params().size() > 1)
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_TOOMANYTARGETS, command) : 0;
	else if (target.find("!") == 0 || target.find("#") == 0 || target.find("&") == 0 || target.find("+") == 0)
	{
		// send message to channel users
		tmp = command.get_cmd_params()[0];
		channel_name = tmp.substr(1);
		for (ch_it = this->__channels.begin(); ch_it != this->__channels.end(); ch_it++)
		{
			if ((*ch_it).get_name() == tmp)
			{
				for (std::list<User>::iterator it2 = this->__users.begin(); it2 != this->__users.end(); it2++)
					send((*it2).get_fd(),  + msg, strlen(msg), 0);
				break ;
			}
		}
		if (ch_it == this->__channels.end())
			write_reply(user, ERR_NOSUCHNICK, command);
	}
	else if (command.get_cmd_params().size() > 0 && !command.get_additional_param().empty())
	{
		if ((receiver = getuserbynick(command.get_cmd_params().front())) == nullptr)
			command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NOSUCHNICK, command) : 0;
		else
		{
			if (receiver->get_modes().get_a())
				command.get_cmd() == "PRIVMSG" ? write_reply(user, RPL_AWAY, command) : 0;
			else
				send(receiver->get_fd(), msg, strlen(msg), 0);
		}
	}
	// ADD A CONDITION FOR WHEN THE CLIENT GIVES A MASK INSTEAD OF USER NICK OR CHANNEL

	return (1);
}

void	Server::LUSERS_handler(msg_parse &command, User &user)
{
	write_reply(user, RPL_LUSERCLIENT, command);
	write_reply(user, RPL_LUSEROP, command);
	write_reply(user, RPL_LUSERUNKNOWN, command);
	write_reply(user, RPL_LUSERCHANNELS, command);
	write_reply(user, RPL_LUSERME, command);
}

void	Server::check_command(msg_parse &command, User &user)
{
	if (command.get_cmd() == "NICK" || command.get_cmd() == "USER" || command.get_cmd() == "PASS")
	{
		user_authentication(command, user);
	}
	else
	{
		if (user.is_real_user())
		{
			if (command.get_cmd() == "MODE")
				MODE_handler(command, user);
			else if (command.get_cmd() == "AWAY")
				AWAY_handler(command, user);
			else if (command.get_cmd() == "WHOIS")
			{
				WHOIS_handler(command, user);
			}
			else if (command.get_cmd() == "LUSERS")
			{
				LUSERS_handler(command, user);
			}
			else if (command.get_cmd() == "PRIVMSG" || command.get_cmd() == "NOTICE")
				PRIVMSG_handler(command, user);
			else if (command.get_cmd() == "QUIT")
				QUIT_handler(user , command);
			else if (command.get_cmd() == "OPER")
				OPER_handler(user, command);
			else if (command.get_cmd() == "TOPIC")
				TOPIC_handler(user, command);
			else if (command.get_cmd() == "JOIN")
				JOIN_handler(user, command);
			else
			{
				write_reply(user, ERR_UNKNOWNCOMMAND, command);
			}
		}
		else
			write_reply(user, ERR_NOTREGISTERED, command);
	}
	// else
	// 	write_socket(user.get_fd(), "Not a valid command\n");
	//check if command is valid 
}

int		Server::write_reply(User &user, int reply_code, msg_parse &command)
{
	if (reply_code == RPL_WELCOME)
	{
		std::string	full_msg =  ":" + this->__name + " 001 :Welcome to the Internet Relay Network\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NICKNAMEINUSE)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd_params().front() + " 433 :Nickname is already in use\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NOWAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 306 :You have been marked as being away\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_UNAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 305 :You are no longer marked as being away\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ERRONEUSNICKNAME)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd_params().front() + "432 :Erroneous nickname\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NONICKNAMEGIVEN)
	{
		std::string	full_msg = ":" + this->__name + "431 :No nickname given\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTREGISTERED)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 451 :You have not registered\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NEEDMOREPARAMS)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 461 :Not enough parameters\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_PASSWDMISMATCH)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 464 :Password incorrect\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ALREADYREGISTRED)
	{
		std::string const_msg = command.get_cmd() + " 462 :Unauthorized command (already registered)\n";
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 462 :Unauthorized command (already registered)\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UMODEUNKNOWNFLAG)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 501 :Unknown MODE flag\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERSDONTMATCH)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 502 :Cannot change mode for other users\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_YOUREOPER)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 381 :You are now an IRC operator\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_TOPIC)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 332 " + command.get_cmd_params()[0] + ":" + command.get_cmd_params()[1] + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_INVITEONLYCHAN)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 473 " + command.get_cmd_params()[0] + ":Cannot join channel (+i)\n" + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BANNEDFROMCHAN)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 474 " + command.get_cmd_params()[0] + ":Cannot join channel (+b)\n" + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BADCHANNELKEY)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 475 " + command.get_cmd_params()[0] + ":Cannot join channel (+k)\n" + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 442 " + command.get_cmd_params()[0] + ":You're not on that channel\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NORECIPIENT)
	{
		std::string const_msg = " 411 :No recipient given (" + command.get_cmd() + ")\n";
		std::string	full_msg = ":" + this->__name + " 411 :No recipient given (" + command.get_cmd() + ")\n" + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTEXTTOSEND)
	{
		std::string const_msg = command.get_cmd() + " 412 :No text to send\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 412 :No text to send\n" + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOSUCHNICK)
	{
		std::string const_msg = command.get_cmd() + " 401 :No such nick/channel\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 401 :No such nick/channel\n" + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_TOOMANYTARGETS)
	{
		std::string const_msg = command.get_cmd() + " 407 :Too many recipients\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 407 :Too many recipients\n" + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_AWAY)
	{
		std::string const_msg = command.get_cmd() + " 301 : " + user.get_away_msg() + "\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 301 : " + user.get_away_msg() + "\n" + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UNKNOWNCOMMAND)
	{
		std::string const_msg = command.get_cmd() + " 421 :Unknown command " ;
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 421 :Unknown command " + user.full_id() + " ";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_WHOISUSER)
	{
		User *tmp = getuserbynick(command.get_cmd_params().front());

		std::string	full_msg = tmp->get_nickname() + " " + tmp->get_username() + " " + tmp->get_hostname() + " * :" + tmp->get_realname() + "\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_WHOISSERVER)
	{
		User *tmp = getuserbynick(command.get_cmd_params().front());

		std::string	full_msg = tmp->get_nickname() + " " + this->__name + " \n"; // ADD SERVER INFO
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_ENDOFWHOIS)
	{
		User *tmp = getuserbynick(command.get_cmd_params().front());

		std::string	full_msg = tmp->get_nickname() + " :End of WHOIS list\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERCLIENT)
	{
		std::string	full_msg = ":There are " + std::to_string(this->__users.size()) + " users\n";// + " and " + number of services + " services on " + number of servers (in our case 1) " servers\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSEROP)
	{
		int	nbr_of_OPs = 0;

		for (std::list<User>::iterator it = this->__users.begin(); it != this->__users.end(); it++)
		{
			if ((*it).get_modes().get_o() || (*it).get_modes().get_O())
				nbr_of_OPs++;
		}
		std::string	full_msg = std::to_string(nbr_of_OPs) + " :operator(s) online\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERUNKNOWN)
	{
		std::string	full_msg = std::to_string(this->__nbr_of_unknown_conns) + " :unknown connection(s)\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERCHANNELS)
	{
		std::string	full_msg = std::to_string(this->__channels.size()) + " :channels formed\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERME)
	{
		std::string	full_msg = ":I have " + std::to_string(this->__users.size()) + " clients\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	// else if (reply_code == RPL_JOINED)
	// {
		
	// }
	return 1;
}

