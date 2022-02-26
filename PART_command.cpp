#include "Server.hpp"
#include "msg_parse.hpp"

void	Server::send_msg_to_channel_users(Channel &chan, std::string &message)
{
	for (std::list<User *>::iterator it = chan.get_users().begin() ; it != chan.get_users().end(); it++)
	{
		write_socket((*it)->get_fd(), message);
	}
}

void	Server::PART_handler(User &user, msg_parse &command)
{
	std::string channel_name;
	int  index = 0;
	int  prev_index = 0;
	std::list<Channel>::iterator chan;

	if ((command.get_cmd_params().size() > 0 && command.get_cmd_params().size() <= 2) || (command.get_cmd_params().size() == 1 && command.get_additional_param().size()))
	{
		std::string _channels = command.get_cmd_params()[0];
		while ((index = _channels.find(',', index) != std::string::npos) || prev_index < _channels.length())
		{
			index = std::string::npos ? index = _channels.length() : 0;
			channel_name = _channels.substr(prev_index, index - prev_index);
			if (find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1)) != __channels.end())
			{
				chan = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
				if (find_user_in_channel(user, *chan) != *((*chan).get_users().end()))
				{
					std::string full_msg = user.full_id() + " :has left the channel" + "\n";
					if (command.get_cmd_params().size() == 2) //send parting message to all users or default message if not given
						full_msg = user.full_id() + " :" + command.get_cmd()[1] + "\n";
					else if (command.get_additional_param().size())
						full_msg = user.full_id() + " :" + command.get_additional_param() + "\n";
					(*chan).remove_user(&user);
					if ((*chan).get_users().size() == 0)
						delete_channel((*chan).get_name());
					// std::cout << "List of users after part :" << std::endl;
					// for (std::list<User *>::iterator it = (*chan).get_users().begin() ; it != (*chan).get_users().end() ; it++)
					// {
					// 	std::cout << (*it)->get_nickname() << std::endl;
					// }
				}
				else
				{
					// std::cout << "not on channel" << std::endl;
					write_reply(user, ERR_NOTONCHANNEL, command);
				}
			}
			else
				write_reply(user, ERR_NOSUCHCHANNEL, command);
			index++;
			prev_index = index;
		}
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}
