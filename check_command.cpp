#include "Server.hpp"

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
				QUIT_handler(user, command);
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
			else if (command.get_cmd() != "PONG")
				write_reply(user, ERR_UNKNOWNCOMMAND, command);
		}
		else
			write_reply(user, ERR_NOTREGISTERED, command);
}
