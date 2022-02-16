#include "msg_parse.hpp"
#include "Server.hpp"

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