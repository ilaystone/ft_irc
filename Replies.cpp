#include "Server.hpp"

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

std::string names_reply(User &user, std::string &serv_name)
{
	return ("\n:" + serv_name + " 353 "+ user.get_nickname() + " channel * :");
}

std::string names_reply(User &user, Channel &channel, std::string &serv_name, char c)
{
	if (c == '=')
		return (":" + serv_name + " 353 "+ user.get_nickname() + " = " + channel.get_name() + " :");
	if (c == '*')
		return (":" + serv_name + " 353 "+ user.get_nickname() + " * " + channel.get_name() + " :");
	if (c == '@')
		return (":" + serv_name + " 353 "+ user.get_nickname() + " @ " + channel.get_name() + " :");
	return "";
}

int		Server::write_reply(User &user, int reply_code, msg_parse &command)
{
	if (reply_code == RPL_WELCOME)
	{
		std::string	full_msg =  ":" + this->__name + " 001 :Welcome to the Internet Relay Network :" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_MYINFO)
	{
		std::string	full_msg =  ":" + this->__name + " 004 Server_name : this is your host " + this->__name + "\n 005 : * Server version : 0.1.\nAvailable Commands :\n-MODE:    <channel> *( ( \"-\"  \"+\" ) *<modes> *<modeparams>.\n-AWAY:    <text>.\n-LUSERS:  <no params>\n-MOTD:    <no params>\n-NAMES:   <channel> *( \",\" <channel> ).\n-LIST:    <channel> *( \",\" <channel> ).\n-PRIVMSG: <msgtarget> <text to be sent>.\n-NOTICE:  <msgtarget> <text>.\n-QUIT:    <Quit Message>.\n-OPER:    <name> <password>.\n-TOPIC:   <channel> [ <topic> ].\n-JOIN:    <channel> *( \",\" <channel> ) [ <key> *( \",\" <key> ) ] / \"0\".\n-PART:    <channel> *( \",\" <channel> ) [ <Part Message> ].\n-INVITE:  <nickname> <channel>.\n-KICK:    <channel> *( \",\" <channel> ) <user> *( \",\" <user> ) [<comment>].\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NICKNAMEINUSE)
	{
		std::string	full_msg = ":" + this->__name + " 433 NICK :Nickname is already in use\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NOWAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 306 AWAY :You have been marked as being away\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_UNAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 305 AWAY :You are no longer marked as being away\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ERRONEUSNICKNAME)
	{
		std::string	full_msg = ":" + this->__name + " 432 " + command.get_cmd_params().front() + " NICK :Erroneous nickname\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NONICKNAMEGIVEN)
	{
		std::string	full_msg = ":" + this->__name + " 431 NICK :No nickname given\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTREGISTERED)
	{
		std::string	full_msg = ":" + this->__name + " 451 " + command.get_cmd() + ":You have not registered\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NEEDMOREPARAMS)
	{
		std::string	full_msg = ":" + this->__name + " 461 " + command.get_cmd() + " :Not enough parameters\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_PASSWDMISMATCH)
	{
		std::string	full_msg = ":" + this->__name+ " 464 " + command.get_cmd() + " :Password incorrect\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ALREADYREGISTRED)
	{
		std::string	full_msg = ":" + this->__name + " 462 " + command.get_cmd() + " :Unauthorized command (already registered)\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UMODEUNKNOWNFLAG)
	{
		std::string	full_msg = ":" + this->__name + " 501 MODE :Unknown MODE flag\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERSDONTMATCH)
	{
		std::string	full_msg = ":" + this->__name + " 502 MODE :Cannot change mode for other users\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_YOUREOPER)
	{
		std::string	full_msg = ":" + this->__name + " 381 OPER :You are now an IRC operator\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_TOPIC)
	{
		std::string	full_msg = ":" + this->__name + " 332 " + command.get_cmd() + command.get_cmd_params()[0] + " :" + command.get_cmd_params()[1] + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_INVITEONLYCHAN)
	{
		std::string	full_msg = ":" + this->__name + " 473 "+ command.get_cmd() + command.get_cmd_params()[0] + " :Cannot join channel (+i)\n" + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BANNEDFROMCHAN)
	{
		std::string	full_msg = ":" + this->__name  + " 474 " + command.get_cmd() + command.get_cmd_params()[0] + " :Cannot join channel (+b)\n" + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_BADCHANNELKEY)
	{
		std::string	full_msg = ":" + this->__name  + " 475 " + command.get_cmd() + command.get_cmd_params()[0] + " :Cannot join channel (+k)\n" + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_CHANNELISFULL)
	{
		std::string	full_msg = ":" + this->__name  + " 471 " + command.get_cmd() + command.get_cmd_params()[0] + " :Cannot join channel (+l)\n" + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name  + " 442 " + command.get_cmd() + command.get_cmd_params()[0] + " :You're not on that channel\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NORECIPIENT)
	{
		std::string	full_msg = ":" + this->__name + " 411 :No recipient given (" + command.get_cmd() + ") \n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_CANNOTSENDTOCHAN)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 404 :Cannot send to channel" + "\n" ;
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTEXTTOSEND)
	{ 
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 412 :No text to send\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOSUCHNICK)
	{
		std::string	full_msg = ":" + this->__name + " 401 * :No such nick/channel\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOSUCHCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + " 403 " + command.get_cmd_params()[0]+ " :No such channel\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_TOOMANYTARGETS)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 407 :Too many recipients\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_AWAY)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 301 : " + command.get_cmd_params()[0] + "\n" + user.get_away_msg() + "\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UNKNOWNCOMMAND)
	{
		std::string	full_msg = ":" + this->__name + " 421 " + command.get_cmd() + " :Unknown command\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERCLIENT)
	{
		std::string	full_msg = ":" + this->__name + " 251 " + user.get_nickname() + " :There are " + std::to_string(this->__users.size()) + " users\n";
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
		std::string	full_msg = ":" + this->__name + " 252 " + user.get_nickname() + " " + std::to_string(nbr_of_OPs) + " :operator(s) online\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERUNKNOWN)
	{
		std::string	full_msg = ":" + this->__name + " 253 " + user.get_nickname() + " " + std::to_string(this->__nbr_of_unknown_conns) + " :unknown connection(s)\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERCHANNELS)
	{
		std::string	full_msg = ":" + this->__name + " 254 " + user.get_nickname() + " " + std::to_string(this->__channels.size()) + " :channels formed\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_LUSERME)
	{
		std::string	full_msg = ":" + this->__name + " 255 " + user.get_nickname() + " :I have " + std::to_string(this->__users.size()) + " clients\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + " 443 " + command.get_cmd_params()[0] + " :is already on channel\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_CHANOPRIVSNEEDED)
	{
		std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 482 " + command.get_cmd_params()[1] + " :You're not channel operator\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOMOTD)
	{
		std::string	full_msg = ":" + this->__name +  " 422 MOTD :MOTD file is missing\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_MOTDSTART)
	{
		std::string	full_msg = ":" + this->__name + " 375 MOTD :- " + this->__name + " Message of the day - \n";
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
				line += "\n";
				full_msg = ":" + this->__name + " 372 MOTD :- " + line;
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				full_msg.clear();
			}
		}
	}
	else if (reply_code == RPL_ENDOFMOTD)
	{
		std::string	full_msg = ":" + this->__name + " 376 MOTD :End of MOTD command\n";
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NAMREPLY)
	{
		std::string	full_msg;
		int			found = 0;

		if (command.get_cmd_params().size() == 0) // LIST ALL CHANNELS AND THEIR USERS (IF VISIBLE)
		{
			for (std::list<Channel>::iterator it = this->__channels.begin(); it != this->__channels.end(); it++)
			{
				if (!(*it).get_modes().get_p() && !(*it).get_modes().get_s())
				{
					full_msg = names_reply(user, *it, this->__name, '=');
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				else if (find_user_in_channel(user, *it) == *((*it).get_users().end()) && !user.get_modes().get_o())
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
						full_msg = (*it).is_operator((*it2)->get_nickname()) == true ? "@" + full_msg : ((*it).has_voice((*it2)->get_nickname()) == true ? "+" + full_msg : full_msg);
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
				if ((*it).get_channels().size() == 0)
				{
					found = -1;
					if (!(*it).get_modes().get_i())
					{
						full_msg = (*it).get_nickname();
						full_msg = (*it).is_channel_op() == true ? "@" + full_msg : ((*it).has_voice() == true ? "+" + full_msg : full_msg);
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
					else if (find_user_in_channel(user, *it) == *((*it).get_users().end()) && !user.get_modes().get_o())
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
							full_msg = (*it).is_operator((*it2)->get_nickname()) == true ? "@" + full_msg : ((*it).has_voice((*it2)->get_nickname()) == true ? "+" + full_msg : full_msg);
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
				if ((!(*it).get_modes().get_p() && !(*it).get_modes().get_s()) || user.get_modes().get_o() || find_user_in_channel(user, (*it)) != *(*it).get_users().end())
				{
					full_msg = ":" + this->__name + " 322 " + user.get_nickname() + " " + (*it).get_prefix() + (*it).get_name() + " " + std::to_string((*it).get_users().size()) + " :" + (*it).get_topic() + "\n";
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					full_msg.clear();
				}
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
					if ((!(*it).get_modes().get_p() && !(*it).get_modes().get_s()) || user.get_modes().get_o() || find_user_in_channel(user, (*it)) != *(*it).get_users().end())
					{
						full_msg = ":" + this->__name + " 322 " + user.get_nickname() + " " + (*it).get_prefix() + (*it).get_name() + " :" + (*it).get_topic() + "\n";
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
						full_msg.clear();
					}
				}
				else
					write_reply(user, ERR_NOSUCHNICK, command);
			}
		}
	}
	else if (reply_code == RPL_LISTEND)
	{
		std::string	full_msg = ":" + this->__name + " 323 " + command.get_cmd() +  " :End of LIST\n";
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