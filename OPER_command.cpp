#include "Server.hpp"
#include "msg_parse.hpp"

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