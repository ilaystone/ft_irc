#include "Server.hpp"
#include "msg_parse.hpp"

void	Server::OPER_handler(User &user, msg_parse &command)
{
	std::string oper_nick = "admin";
	std::string oper_pass = "admin";

	if (command.get_cmd_params().size() > 1)
	{
		if (command.get_cmd_params().size() > 1 && command.get_cmd_params()[1] == oper_pass && command.get_cmd_params()[0] == oper_nick)
		{
			user.set_modes('o');
			user.set_modes('O');
			write_reply(user, RPL_YOUREOPER, command);
		}
		else if (command.get_cmd_params()[1] != oper_pass)
			write_reply(user, ERR_PASSWDMISMATCH, command);
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}