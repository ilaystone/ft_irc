#include "msg_parse.hpp"
#include "Server.hpp"
#include <time.h>

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
			if ((*ch_it).get_name() == channel_name)
			{
				if (((*ch_it).get_modes().get_n() == true && (find_user_in_channel(user, *ch_it) == *(*ch_it).get_users().end() && user.get_modes().get_o() == false))
				|| ((*ch_it).get_modes().get_m() == true && (!(*ch_it).is_operator(user.get_nickname()) && !(*ch_it).has_voice(user.get_nickname()) && user.get_modes().get_o() == false)))
				{
					write_reply(user, ERR_CANNOTSENDTOCHAN, command);
					break ;
				}
				for (std::list<User *>::iterator it2 = (*ch_it).get_users().begin(); it2 != (*ch_it).get_users().end(); it2++)
					if (user.get_fd() != (*it2)->get_fd())
						send((*it2)->get_fd(),  + msg, strlen(msg), 0);
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

void	Server::MOTD_handler(msg_parse &command, User &user)
{
	write_reply(user, RPL_MOTDSTART, command);
	write_reply(user, RPL_MOTD, command);
	write_reply(user, RPL_ENDOFMOTD, command);
}

void	Server::NAMES_handler(msg_parse &command, User &user)
{
	write_reply(user, RPL_NAMREPLY, command);
	write_reply(user, RPL_ENDOFNAMES, command);
}

void	Server::LIST_handler(msg_parse &command, User &user)
{
	write_reply(user, RPL_LIST, command);
	write_reply(user, RPL_LISTEND, command);
}

void	Server::check_command(msg_parse &command, User &user)
{
	if (command.get_cmd() == "NICK" || command.get_cmd() == "USER" || command.get_cmd() == "PASS")
	{
		user_authentication(command, user);
	}
	else if (user.is_real_user())
	{
			if (command.get_cmd() == "MODE")
				MODE_handler(command, user);
			else if (command.get_cmd() == "AWAY")
				AWAY_handler(command, user);
			else if (command.get_cmd() == "WHOIS")
				WHOIS_handler(command, user);
			else if (command.get_cmd() == "LUSERS")
				LUSERS_handler(command, user);
			else if (command.get_cmd() == "MOTD")
				MOTD_handler(command, user);
			else if (command.get_cmd() == "NAMES")
				NAMES_handler(command, user);
			else if (command.get_cmd() == "LIST")
				LIST_handler(command, user);
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
			else if (command.get_cmd() == "PART")
				PART_handler(user, command);
			else if (command.get_cmd() == "INVITE")
				INVITE_handler(user, command);
			else if (command.get_cmd() == "KICK")
				KICK_handler(user, command);
			else
				write_reply(user, ERR_UNKNOWNCOMMAND, command);
	}
	else
		write_reply(user, ERR_NOTREGISTERED, command);
	// else
	// 	write_socket(user.get_fd(), "Not a valid command\n");
	//check if command is valid 
}

std::string names_reply(User &user, Channel &channel, std::string &serv_name, char c)
{
	if (c == '=')
		return (":" + serv_name + " " + user.get_nickname() + " = " + channel.get_name() + " :");
	if (c == '*')
		return (":" + serv_name + " " + user.get_nickname() + " * " + channel.get_name() + " :");
	if (c == '@')
		return (":" + serv_name + " " + user.get_nickname() + " @ " + channel.get_name() + " :");
	return "";
}

std::string names_reply(User &user, std::string &serv_name)
{
	return ("\n:" + serv_name + " " + user.get_nickname() + " channel * :");
}

int		Server::write_reply(User &user, int reply_code, msg_parse &command)
{
	if (reply_code == RPL_WELCOME)
	{
		std::string	full_msg =  ":" + this->__name + " 001 :Welcome to the Internet Relay Network :" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NICKNAMEINUSE)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd_params().front() + " 433 :Nickname is already in use\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NOWAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 306 :You have been marked as being away\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_UNAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 305 :You are no longer marked as being away\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ERRONEUSNICKNAME)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd_params().front() + "432 :Erroneous nickname\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NONICKNAMEGIVEN)
	{
		std::string	full_msg = ":" + this->__name + "431 :No nickname given\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTREGISTERED)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 451 :You have not registered\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NEEDMOREPARAMS)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 461 :Not enough parameters\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_PASSWDMISMATCH)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 464 :Password incorrect\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ALREADYREGISTRED)
	{
		// std::string const_msg = command.get_cmd() + " 462 :Unauthorized command (already registered)\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 462 :Unauthorized command (already registered)\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UMODEUNKNOWNFLAG)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 501 :Unknown MODE flag\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERSDONTMATCH)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 502 :Cannot change mode for other users\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_YOUREOPER)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 381 :You are now an IRC operator\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_TOPIC)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 332 " + command.get_cmd_params()[0] + " :" + command.get_cmd_params()[1] + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	// else if (reply_code == RPL_INVITING)
	// {
	// 	std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 341 " + command.get_cmd_params()[1] + " :" + command.get_cmd_params()[0] + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
	// 	send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	// }
	else if (reply_code == ERR_INVITEONLYCHAN)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 473 " + command.get_cmd_params()[0] + " :Cannot join channel (+i)\n" + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BANNEDFROMCHAN)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 474 " + command.get_cmd_params()[0] + " :Cannot join channel (+b)\n" + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BADCHANNELKEY)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 475 " + command.get_cmd_params()[0] + " :Cannot join channel (+k)\n" + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 442 " + command.get_cmd_params()[0] + " :You're not on that channel\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NORECIPIENT)
	{
		// std::string const_msg = " 411 :No recipient given (" + command.get_cmd() + ")\n";
		std::string	full_msg = ":" + this->__name + " 411 :No recipient given (" + command.get_cmd() + ") \n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_CANNOTSENDTOCHAN)
	{
		std::string	full_msg = ":" + this->__name + " 404 :Cannot send to channel" + "\n" /*+ user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTEXTTOSEND)
	{ 
		// std::string const_msg = command.get_cmd() + " 412 :No text to send\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 412 :No text to send\n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOSUCHNICK)
	{
		// std::string const_msg = command.get_cmd() + " 401 :No such nick/channel\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 401 :No such nick/channel\n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOSUCHCHANNEL)
	{
		// std::string const_msg = command.get_cmd() + " 401 :No such nick/channel\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd_params()[0] + " 403 :No such channel\n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_TOOMANYTARGETS)
	{
		// std::string const_msg = command.get_cmd() + " 407 :Too many recipients\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 407 :Too many recipients\n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_AWAY)
	{
		// std::string const_msg = command.get_cmd() + " 301 : " + user.get_away_msg() + "\n";
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 301 : " + command.get_cmd_params()[0] + "\n" + user.get_away_msg() + "\n"/* + user.full_id() + " "*/;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UNKNOWNCOMMAND)
	{
		// std::string const_msg = command.get_cmd() + " 421 :Unknown command " ;
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 421 :Unknown command\n"/* + user.full_id() + "\n"*/;
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
		std::string	full_msg = ":" + this->__name + " :There are " + std::to_string(this->__users.size()) + " users\n";// + " and " + number of services + " services on " + number of servers (in our case 1) " servers\n";
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
		std::string	full_msg = ":" + this->__name + " " + std::to_string(nbr_of_OPs) + " :operator(s) online\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERUNKNOWN)
	{
		std::string	full_msg = ":" + this->__name + " " + std::to_string(this->__nbr_of_unknown_conns) + " :unknown connection(s)\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERCHANNELS)
	{
		std::string	full_msg = ":" + this->__name + " " + std::to_string(this->__channels.size()) + " :channels formed\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERME)
	{
		std::string	full_msg = ":" + this->__name + " :I have " + std::to_string(this->__users.size()) + " clients\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 443 " + command.get_cmd_params()[0] + " "+ command.get_cmd_params()[1] + " :is already on channel\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_CHANOPRIVSNEEDED)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 482 " + command.get_cmd_params()[1] + " :You're not channel operator\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOMOTD)
	{
		std::string	full_msg = ":" + this->__name + " :MOTD file is missing\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_MOTDSTART)
	{
		std::string	full_msg = ":- " + this->__name + " Message of the day -\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_MOTD)
	{
		std::ifstream motd_file;
		std::string	line;
		std::string	full_msg;

		motd_file.open("motd.txt", std::ifstream::in);
		if (!motd_file)
			write_reply(user, ERR_NOMOTD, command);
		else
		{
			while (std::getline(motd_file, line))
			{
				// check if line's length is bigger than 80 characters
				line += "\n";
				full_msg = ":" + this->__name + " - " + line;
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				full_msg.clear();
			}
		}
	}
	else if (reply_code == RPL_ENDOFMOTD)
	{
		std::string	full_msg = ":" + this->__name + " :End of MOTD command\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NAMREPLY)
	{
		// NEEDS TO BE RE-CHECKED
		std::string	full_msg;
		int			found = 0;

		if (command.get_cmd_params().size() == 0) // LIST ALL CHANNELS AND THEIR USERS (IF VISIBLE)
		{
			for (std::list<Channel>::iterator it = this->__channels.begin(); it != this->__channels.end(); it++)
			{
				// maybe check if the channel is visible or not
				if (!(*it).get_modes().get_p() && !(*it).get_modes().get_s())
				{
					full_msg = names_reply(user, *it, this->__name, '=');
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				else if (find_user_in_channel(user, *it) == *((*it).get_users().end()))
					continue ;
				if ((*it).get_modes().get_p())
				{
					full_msg = names_reply(user, *it, this->__name, '*');
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				else if ((*it).get_modes().get_s())
				{
					full_msg = names_reply(user, *it, this->__name, '@');
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				for(std::list<User *>::iterator it2 = (*it).get_users().begin(); it2 != (*it).get_users().end(); it2++)
				{
					if (!(*it2)->get_modes().get_i())
					{
						full_msg = (*it2)->get_nickname();
						full_msg = (*it).is_operator((*it2)->get_nickname()) == true ? "@" + full_msg : full_msg;
						full_msg = (*it2) != (*it).get_users().back() ? full_msg + " " : full_msg + "\n";
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
						found = -1;
					}
					else
						found = 0;
				}
			}
			// LIST USERS THAT ARE VISIBLE BUT NOT ON ANY CHANNEL OR ON A NONE VISIBLE CHANNEL
			full_msg = names_reply(user, this->__name);
			send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			for(std::list<User>::iterator it = this->__users.begin(); it != this->__users.end(); it++)
			{
				if ((*it).get_channels().size() == 0) // OR CHANNEL IS NOT VISIBLE
				{
					found = -1;
					if (!(*it).get_modes().get_i())
					{
						full_msg = (*it).get_nickname();
						full_msg = (*it).is_channel_op() == true ? "@" + full_msg : full_msg;
						full_msg = *it != this->__users.back() ? full_msg + " " : full_msg + "\n";
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
				}
				else
					found = 0;
			}
			if (found == 0)
				send(user.get_fd(), "\n", 1, 0);
		}
		else // LIST THE GIVEN CHANNEL'S (OR MULTIPLE CHANNELS) NAME(s) AND USERS 
		{
			std::string ch_name = command.get_cmd_params()[0];
			std::string name;
			std::string full_name;
			std::list<Channel>::iterator it;

			while (!(name = split_channel_names(ch_name)).empty())
			{
				if ((it = find_channel(name[0], name.substr(1, name.length() - 1))) != this->__channels.end())
				{
					full_name = (*it).get_prefix() + (*it).get_name();
					if (!(*it).get_modes().get_p() && !(*it).get_modes().get_s())
					{
						full_msg = names_reply(user, *it, this->__name, '=');
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
					else if (find_user_in_channel(user, *it) == *((*it).get_users().end()))
						continue ;
					if ((*it).get_modes().get_p())
					{
						full_msg = names_reply(user, *it, this->__name, '*');
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
					else if ((*it).get_modes().get_s())
					{
						full_msg = names_reply(user, *it, this->__name, '@');
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
					for(std::list<User *>::iterator it2 = (*it).get_users().begin(); it2 != (*it).get_users().end(); it2++)
					{
						if (!(*it2)->get_modes().get_i())
						{
							full_msg = (*it2)->get_nickname();
							full_msg = (*it).is_operator((*it2)->get_nickname()) == true ? "@" + full_msg : full_msg;
							full_msg = (*it2) != (*it).get_users().back() ? full_msg + " " : full_msg + "\n";
							send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
							found = -1;
						}
						else
							found = 0;	
					}
				}
				else
					write_reply(user, ERR_NOSUCHNICK, command);
				// sleep(1);
			}
		}
	}
	else if (reply_code == RPL_ENDOFNAMES)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() +  " 366 :End of Names command\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LIST)
	{
		std::string	full_msg;
		std::string	ch_name;

		if (command.get_cmd_params().size() == 0)
		{
			for(std::list<Channel>::iterator it = this->__channels.begin(); it != this->__channels.end(); it++)
			{
				full_msg = ":" + this->__name + " " + (*it).get_name() + " <#visible> " + ":" + (*it).get_topic() + "\n";
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			} 
		}
		else
		{
			std::list<Channel>::iterator it;
			std::string name;

			ch_name = command.get_cmd_params()[0];
			while (!(name = split_channel_names(ch_name)).empty())
			{
				if ((it = find_channel(name[0], name.substr(1, name.size() - 1))) != this->__channels.end())
				{
					full_msg = ":" + this->__name + " " + (*it).get_name() + " <#visible> " + ":" + (*it).get_topic() + "\n";
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				else
					write_reply(user, ERR_NOSUCHNICK, command);
			}
		}
	}
	else if (reply_code == RPL_LISTEND)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() +  " 323 :End of LIST\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UNKNOWNMODE)
	{
		std::string	full_msg = command.get_cmd() +  " 472 " + command.get_cmd_params()[1][command.get_pos() + 1] + " is unknown mode char to me for " + (command.get_cmd_params()[0] + 1) + "\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERNOTINCHANNEL)
	{
		std::string	full_msg;

		if (command.get_cmd_params().size() == 3)
			full_msg = command.get_cmd() +  " 441 " + command.get_cmd_params()[2] + " " + command.get_cmd_params()[0] + " :they aren't on that channel\n";
		else
			full_msg = command.get_cmd() +  " 441 " + user.get_nickname() + " " + command.get_cmd_params()[0] + " :they aren't on that channel\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_KEYSET)
	{
		std::string	full_msg = command.get_cmd() +  " 467 " + command.get_cmd_params()[0] + " :Channel key already set" + "\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_CHANNELMODEIS)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() +  " 324 " + command.get_cmd_params()[0] + " " + command.get_cmd_params()[1][0] + command.get_cmd_params()[1][command.get_pos()];
		full_msg = command.get_cmd_params().size() == 3 ? full_msg +  " " + command.get_cmd_params()[2] + "\n" : full_msg + "\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	return 1;
}

