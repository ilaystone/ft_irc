#include "Server.hpp"

int		Server::PRIVMSG_handler(msg_parse &command, User &user)
{
	User	*receiver;
	std::string	channel_name;
	std::list<Channel>::iterator ch_it;
	std::string params = command.get_cmd() + " ";
	std::string msg;

	if (command.get_cmd_params().size() < 1)
	{
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NORECIPIENT, command) : 0;
		return 0;
	}
	else
		channel_name = command.get_cmd_params()[0];
	if (command.get_additional_param().size() == 0 && command.get_cmd_params().size() < 2)
	{
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NOTEXTTOSEND, command) : 0;
		return 0;
	}
	if ((command.get_cmd_params().size() > 1 && command.get_additional_param().size()) || (command.get_cmd_params().size() > 2 && !command.get_additional_param().size()))
	{
		command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_TOOMANYTARGETS, command) : 0;
		return 0;
	}
	else if (channel_name.find("#") == 0 || channel_name.find("+") == 0)
	{
		// send message to all channel users
		params = params + command.get_cmd_params().front() + " :" + (command.get_cmd_params().size() > 1 ? (command.get_cmd_params()[1]) : command.get_additional_param()) + "\n";
		channel_name = channel_name.substr(1);
		for (ch_it = this->__channels.begin(); ch_it != this->__channels.end(); ch_it++)
		{
			if ((*ch_it).get_name() == channel_name)
			{
				if (((*ch_it).get_modes().get_n() == true && (find_user_in_channel(user, *ch_it) == *(*ch_it).get_users().end() && user.get_modes().get_o() == false))
				|| ((*ch_it).get_modes().get_m() == true && (!(*ch_it).is_operator(user.get_nickname()) && !(*ch_it).has_voice(user.get_nickname()) && user.get_modes().get_o() == false))
				|| ((((*ch_it).get_modes().get_p() == true || (*ch_it).get_modes().get_s() == true)) && find_user_in_channel(user, *ch_it) == *(*ch_it).get_users().end() && user.get_modes().get_o() == false))
				{
					write_reply(user, ERR_CANNOTSENDTOCHAN, command);
					break ;
				}
				for (std::list<User *>::iterator it2 = (*ch_it).get_users().begin(); it2 != (*ch_it).get_users().end(); it2++)
					if (user.get_fd() != (*it2)->get_fd())
					{
						msg = (*ch_it).get_modes().get_a() == true ? ":anonymous!anonymous@anonymous " + params : ":" + user.full_id() + " " + msg + params;
						send((*it2)->get_fd(), msg.c_str(), msg.size(), 0);
						msg.clear();
					}
				break ;
			}
		}
		if (ch_it == this->__channels.end())
			write_reply(user, ERR_NOSUCHNICK, command);
	}
	else if (command.get_cmd_params().size() > 0)
	{
		msg = ":" + user.full_id() + " " + params + command.get_cmd_params().front() + " :" + (command.get_cmd_params().size() > 1 ? (command.get_cmd_params()[1]) : command.get_additional_param()) + "\n";
		if ((receiver = getuserbynick(command.get_cmd_params().front())) == nullptr)
			command.get_cmd() == "PRIVMSG" ? write_reply(user, ERR_NOSUCHNICK, command) : 0;
		else
		{
			if (receiver->get_modes().get_a())
			{
				std::string	full_msg = ":" + this->__name + " 301 " + command.get_cmd() + " " + receiver->get_nickname() + " :" + receiver->get_away_msg() + "\n"/* + user.full_id() + " "*/;
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
			else
				send(receiver->get_fd(), msg.c_str(), msg.size(), 0);
		}
	}
	return (1);
}