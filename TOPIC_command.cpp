#include "msg_parse.hpp"
#include "Server.hpp"

void	Server::TOPIC_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() >= 1)
	{
		// if (command.get_cmd_params().size() == 1)
		// {
		// 	// std::string	full_msg = ":" + this->__name + command.get_cmd() + " 332 " + command.get_cmd_params()[0] + ":" + command.get_cmd_params()[1] + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
		// 	// send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
		// 	write_reply(user, RPL_TOPIC, command);
		// }
		// else							//should be checked when there is a channel 
		// {
			Channel chan;
			if (find_channel(command.get_cmd_params()[0][0], command.get_cmd_params()[0]) != __channels.end())
			{
				std::string str = command.get_cmd_params()[0];
				str.erase(0, 1);
				// std::cout << "this is the string after the prefix was deleted " << str << "|" << std::endl;
				chan = *find_channel(command.get_cmd_params()[0][0], str);
				if (command.get_cmd_params().size() == 1)
				{
					if (chan.get_topic().empty())
					{
						std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 331 " + chan.get_name() + " :No topic is set\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
					else
					{
						std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 332 " + chan.get_name() + " :" + chan.get_topic() +"\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					}
				}
				if (find_user_in_channel(user, chan) == *chan.get_users().end())
				{
					write_reply(user, ERR_NOTONCHANNEL, command);
					return ;
				}
				if (!(/*!user.get_modes().get_a() && chan.get_modes().get_i()*/ chan.get_modes().get_t() && !is_operator_on_channel(user, chan)))
				{
					if (command.get_cmd_params().size() == 2)
						chan.set_topic(command.get_cmd_params()[1]);
					else if (command.get_additional_param().size())
						chan.set_topic(command.get_additional_param());
					write_reply(user, RPL_TOPIC, command);
				}
				else
				{
					std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 482 " + chan.get_name() + " :You're not channel operator\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					// write_reply(user, ERR_CHANOPRIVSNEEDED, command);
				}
			}
			else
				// write_reply(user, ERR_NOSUCHCHANNEL, command)
				write_socket(user.get_fd(), "Channel not found\n");
		// }
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}
