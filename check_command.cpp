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
			// std::cout << "this is the size of real name" << user.get_realname().size() << std::endl;
			if (command.get_cmd_params().size() == 4 || command.get_cmd_params().size() == 3)
			{
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
	// std::cour 
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

void	Server::check_command(msg_parse &command, User &user)
{
	if ((command.get_cmd() == "NICK" || command.get_cmd() == "USER" || command.get_cmd() == "PASS"))
	{
		user_authentication(command, user);
	}
	else
	{
		if (user.is_real_user())
		{
			if (command.get_cmd() == "MODE")
			{
				MODE_handler(command, user);
			}
			else if (command.get_cmd() == "AWAY")
			{
				AWAY_handler(command, user);
			}
			else if (command.get_cmd() == "PRIVMSG")
			{
				PRIVMSG_handler(command, user);
			}
			else if (command.get_cmd() == "QUIT")
			{
				QUIT_handler(user , command);
			}
		}
		else
		{
			write_reply(user, ERR_NOTREGISTERED, command);
		}
	}
	// else
	// 	write_socket(user.get_fd(), "Not a valid command\n");
	//check if command is valid 
}

int		Server::write_reply(User &user, int reply_code, msg_parse &command) const
{
	if (reply_code == RPL_WELCOME)
	{
		std::string	full_msg =  ":" + this->__name + " 001 :Welcome to the Internet Relay Network\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NICKNAMEINUSE)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd_params().front() + " 433 :Nickname is already in use\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_NOWAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 306 :You have been marked as being away\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == RPL_UNAWAY)
	{
		std::string	full_msg = ":" + this->__name + " 305 :You are no longer marked as being away\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ERRONEUSNICKNAME)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd_params().front() + "432 :Erroneous nickname\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NONICKNAMEGIVEN)
	{
		std::string	full_msg = ":" + this->__name + "431 :No nickname given\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NOTREGISTERED)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 451 :You have not registered\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_NEEDMOREPARAMS)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 461 :Not enough parameters\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_ALREADYREGISTRED)
	{
		std::string const_msg = command.get_cmd() + " 462 :Unauthorized command (already registered)\n";
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 462 :Unauthorized command (already registered)\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_UMODEUNKNOWNFLAG)
	{
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 501 :Unknown MODE flag\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	else if (reply_code == ERR_USERSDONTMATCH)
	{
		std::string const_msg = command.get_cmd() + " 502 :Cannot change mode for other users\n";
		std::string	full_msg = ":" + this->__name + command.get_cmd() + " 502 :Cannot change mode for other users\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname(); 
		send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
	}
	return 1;
}

