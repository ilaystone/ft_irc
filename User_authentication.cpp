#include "Server.hpp"
#include "msg_parse.hpp"

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
		this->dec_nbr_of_unknown_conns();
	}
}