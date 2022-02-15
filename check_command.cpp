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

void	Server::user_authentication( msg_parse &command, User &user)
{
	if (command.get_cmd() == "NICK")
	{
		if (command.get_cmd_params().size() == 1)
		{
			if (strlen(command.get_cmd_params().front()) <= 9 && check_for_bad_char(command.get_cmd_params().front()))
			{
				if (__list_nicks.insert(command.get_cmd_params().front()).second)
					user.set_nickname(command.get_cmd_params().front());
				else 
					write_reply(user, ERR_NICKNAMEINUSE, command);
			}
			else
				write_reply(user, ERR_ERRONEUSNICKNAME, command);

		}
		else
			write_reply(user, ERR_NONICKNAMEGIVEN, command);

	}
	else if (command.get_cmd() == "USER")
	{
		if (!user.get_realname().size())
		{
			if (command.get_cmd_params().size() == 4 || command.get_cmd_params().size() == 3)
			{
				// mode implementation stays
				// {
				// // std::string str = "MODE " + user.get_nickname() + " " + command.get_cmd_params()[1]; //ugly i kknow
				// // msg_parse mode_msg(str);			
				// // mode_msg.parser();
				// // user_mode_setter(mode_msg, user);
				// }
				user.set_username(command.get_cmd_params().front());
				/* <user> <mode> <unused> <realname>*/
				// user.set_modes((int)command.get_cmd_params()[2]);
				if (command.get_cmd_params().size() == 4)
					user.set_realname(command.get_cmd_params()[3]);
				else
					user.set_realname(command.get_additional_param());
			}
			else
			{
				write_reply(user, ERR_NEEDMOREPARAMS, command);
			}
		}
		else
			write_reply(user, ERR_ALREADYREGISTRED, command);
		// std::cout << user.get_username() << std::endl;
	}
	else if (command.get_cmd() == "PASS")
	{
		if (user.get_pass_check() && command.get_cmd_params().size())
		{
			write_reply(user, ERR_ALREADYREGISTRED, command);
		}
		else if (command.get_cmd_params().size() == 1 && __password == *command.get_cmd_params().begin())
		{
			std::cout << "PASS CHECKED" << std::endl;
			user.set_pass_check(TRUE);
		}
		else if (command.get_cmd_params().size() == 0)
		{
			write_reply(user, ERR_NEEDMOREPARAMS, command);
		}
	}
	if (!user.get_nickname().empty()  && !user.get_username().empty() && user.get_pass_check() == 1)
	{
		user.set_is_real_user(TRUE);
		write_reply(user, RPL_WELCOME, command);
		user.set_pass_check(2);
	}
}

int		Server::PRIVMSG_handler(msg_parse &command, User &user)
{
	// if ()
	// {

	// }
	return (1);
}

int		Server::user_mode_setter(msg_parse &command, User &user)
{
	if (command.get_cmd_params().size() >= 2)
	{
		if (command.get_cmd_params()[0] == user.get_nickname())
		{
			if (command.get_cmd_params()[1][0] == '+')
			{
				for (int i = 1; i < strlen(command.get_cmd_params()[1]); i++)
				{
					if (command.get_cmd_params()[1][i] == 'i' || command.get_cmd_params()[1][i] == 'w' || command.get_cmd_params()[1][i] == 'r' || command.get_cmd_params()[1][i] == 's')
						user.set_modes(command.get_cmd_params()[1][i]);
					else if (command.get_cmd_params()[1][i] != 'a' && command.get_cmd_params()[1][i] != 'o' && command.get_cmd_params()[1][i] != 'O')
					{
						write_reply(user, ERR_UMODEUNKNOWNFLAG, command);
						break ;
					}
				}
				
			}
			else if (command.get_cmd_params()[1][0] == '-')
			{
				for (int i = 1; i < strlen(command.get_cmd_params()[1]); i++)
				{
					if (command.get_cmd_params()[1][i] == 'i' || command.get_cmd_params()[1][i] == 'w' || command.get_cmd_params()[1][i] == 'o' || command.get_cmd_params()[1][i] == 'O' || command.get_cmd_params()[1][i] == 's')
						user.unset_modes(command.get_cmd_params()[1][i]);
					else if (command.get_cmd_params()[1][i] != 'a' && command.get_cmd_params()[1][i] != 'r')
					{
						write_reply(user, ERR_UMODEUNKNOWNFLAG, command);
						break ;
					}
				}
			}
		}
		else
			write_reply(user, ERR_USERSDONTMATCH, command);
	}
	else if (command.get_cmd_params().size() == 1)
		write_socket(user.get_fd(), "The available modes are as follows:\na - user is flagged as away;\ni - marks a users as invisible;\nw - user receives wallops;\nr - restricted user connection;\no - operator flag;\nO - local operator flag;\ns - marks a user for receipt of server notices.\n");
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
	return (1);
}

int		Server::MODE_handler(msg_parse &command, User &user)
{
	if (command.get_cmd_params().size() &&  __list_nicks.find(command.get_cmd_params().front()) != __list_nicks.end())
	{
		user_mode_setter( command, user);
	}
	else
	{
		write_reply(user, ERR_USERSDONTMATCH, command);
	}
	return (1);
}

int		Server::AWAY_handler(msg_parse &command, User &user)
{
	if (command.get_additional_param().size())
	{
		user.set_modes(1);
		user.set_away_msg(command.get_additional_param());
		write_reply(user, RPL_NOWAWAY, command);
	}
	else
	{
		user.unset_modes(1);
		write_reply(user, RPL_UNAWAY, command);
	}
	return (1);
}

void	Server::QUIT_handler(User &user, msg_parse &command)
{
	std::string full_msg = "Closing Link: HOST_NAME";

	// if (command.get_cmd_params().size())
	// 	full_msg = ":" + this->__name + "@" + command.get_cmd_params().front() + " 433 :Nickname is already in use\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname();
	write_socket(user.get_fd(), full_msg);
	this->disconnect_user(user);
}

void	Server::OPER_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() >= 1)
	{
		// <username(will be ignored)> <(server)password>
		if (command.get_cmd_params().size() > 1 && command.get_cmd_params()[1] == this->__password)
		{
			user.set_modes('o');
			user.set_modes('O');
			write_reply(user, RPL_YOUREOPER, command);
		}
		else
			write_reply(user, ERR_PASSWDMISMATCH, command);
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
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

void	Server::TOPIC_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() >= 1)
	{
		if (command.get_cmd_params().size() == 1)
		{
			// std::string	full_msg = ":" + this->__name + command.get_cmd() + " 332 " + command.get_cmd_params()[0] + ":" + command.get_cmd_params()[1] + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
			// send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			write_reply(user, RPL_TOPIC, command);
		}
		else							//should be checked when there is a channel 
		{
			Channel chan;
			if (find_channel(command.get_cmd_params()[0][0], command.get_cmd_params()[0]) != __channels.end())
			{
				std::string str = command.get_cmd_params()[0];
				str.erase(0, 1);
				std::cout << "this is the string after the prefix was deleted " << str << "|" << std::endl;
				chan = *find_channel(command.get_cmd_params()[0][0], str);
				if (find_user_in_channel(user, chan) == *chan.get_users().end())
				{
					write_reply(user, ERR_NOTONCHANNEL, command);
					return ;
				}
				if (!(!user.get_modes().get_a() && chan.get_modes().get_i()))
				{
					if (command.get_cmd_params().size() == 2)
						chan.set_topic(command.get_cmd_params()[1]);
					else if (command.get_additional_param().size())
						chan.set_topic(command.get_additional_param());
					write_reply(user, RPL_TOPIC, command);
				}
			}
			else
				write_socket(user.get_fd(), "Channel not found\n");
		}
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}

void		Server::JOIN_handler(User &user, msg_parse &command)
{
	std::string channels = command.get_cmd_params()[0];
	if (channels == "0")
	{
		if (user.get_nb_channels())
		{
			//part
			std::cout << "User should leave all channels" << std::endl;
		}
	}
	if (command.get_cmd_params().size() == 2)
	{
		std::string keys = command.get_cmd_params()[1];
		size_t channel_index = 0;
		int prev_channel_index = 0;
		size_t key_index = 0;
		int prev_key_index = 0;
		int prev_chan_index = 0;
		std::string sub_string;
		while ((channel_index = channels.find(',', channel_index)) != std::string::npos)
		{
			while ((key_index = keys.find(',', key_index)) != std::string::npos)
			{
				// sub_string = channels.substr(prev_key_index, key_index - prev_key_index + 1);
				// if (find_channel(sub_string[0], sub_string.substr(1, sub_string.length() - 1)) == __channels.end())
				// {
				// 	// Channel new_chan(sub_string[0], sub_string.substr(1, sub_string.length() - 1) , "", 1);
				// 	// new_chan.set_password(sub_string);
				// 	add_channel(sub_string.substr(1, sub_string.length() - 1), sub_string);
				// 	Channel chan = *find_channel(sub_string[0], sub_string.substr(1, sub_string.length() - 1));
				// 	// chan.set
				// 	// user.set_modes('o');
				// 	// user.set_modes('O');
				// }
				// else
				// {
				// 	std::cout << "check if key is right and join channle" << std::endl;
				// }
				key_index++;
				prev_key_index = key_index;
			}
			channel_index++;
			prev_chan_index = channel_index;
		}
	}
	else
		write_socket(user.get_fd(), "Wrong number of params\n");
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
			else if (command.get_cmd() == "PRIVMSG")
				PRIVMSG_handler(command, user);
			else if (command.get_cmd() == "QUIT")
				QUIT_handler(user , command);
			else if (command.get_cmd() == "OPER")
				OPER_handler(user, command);
			else if (command.get_cmd() == "TOPIC")
				TOPIC_handler(user, command);
			else if (command.get_cmd() == "JOIN")
				JOIN_handler(user, command);
		}
		else
			write_reply(user, ERR_NOTREGISTERED, command);
	}
	// else
	// 	write_socket(user.get_fd(), "Not a valid command\n");
	//check if command is valid 
}

int		Server::write_reply(User &user, int reply_code, msg_parse &command) const
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
	else if (reply_code == ERR_NOTONCHANNEL)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 442 " + command.get_cmd_params()[0] + ":You're not on that channel\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	return 1;
}

