#include "msg_parse.hpp"
#include "Server.hpp"

void	Server::TOPIC_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() >= 1)
	{
		std::list<Channel>::iterator chan;
		std::string chan_name = command.get_cmd_params().front();
		int			unset = 0;

		if (chan_name[chan_name.length() - 1] == '.')
		{
			unset = 1;
			chan_name = chan_name.substr(0, chan_name.length() - 1);
		}
		if ((chan = find_channel(command.get_cmd_params()[0][0], chan_name.substr(1, chan_name.length() - 1))) != __channels.end())
		{
			if (find_user_in_channel(user, *chan) == *(*chan).get_users().end() && !user.get_modes().get_o())
			{
				write_reply(user, ERR_NOTONCHANNEL, command);
				return ;
			}
			if (command.get_cmd_params().size() == 1 && !command.get_additional_param().size())
			{
				if (unset || command.get_has_additional_param())
					(*chan).set_topic("");
				else if ((*chan).get_topic().empty())
				{
					std::string	full_msg = ":" + this->__name + " 331 " + command.get_cmd() + " " + command.get_cmd_params().front() + " :No topic is set\n"; 
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					return ;
				}
				else
				{
					std::string	full_msg = ":" + this->__name + " 332 " + command.get_cmd() + " " + command.get_cmd_params().front() + " :" + (*chan).get_topic() +"\n" ; 
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					return ;
				}
			}
			if (!((*chan).get_modes().get_t() && !is_operator_on_channel(user, *chan)) || user.get_modes().get_o())
			{
				if (command.get_cmd_params().size() == 2)
					(*chan).set_topic(command.get_cmd_params()[1]);
				else if (!command.get_additional_param().empty())
					(*chan).set_topic(command.get_additional_param());
				std::string	full_msg = ":" + this->__name + " 332 " + command.get_cmd() + " " + command.get_cmd_params().front() + " :" + (*chan).get_topic() +"\n"; 
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
			else
			{
				std::string	full_msg = ":" + this->__name + " 482 " + command.get_cmd_params().front() + " :You're not channel operator\n"; 
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
		}
		else
			write_reply(user, ERR_NOSUCHCHANNEL, command);
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}

