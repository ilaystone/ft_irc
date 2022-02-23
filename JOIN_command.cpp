#include "msg_parse.hpp"
#include "Server.hpp"

void		Server::part_from_all_channels(User &user)
{
	for (std::list<Channel *>::iterator it = user.get_channels().begin() ; it != user.get_channels().end(); it++)
	{
		std::string name = (*it)->get_prefix() + (*it)->get_name();
		std::string buff = "PART " + name;
		msg_parse command(buff);
		command.parser();
		PART_handler(user, command);
		// (*it)->remove_user(&user);
		user.remove_channel(*it);
	}
}

void		Server::JOIN_handler(User &user, msg_parse &command)
{
	std::list<Channel>::iterator chan;
	std::string channels;

	if (command.get_cmd_params().size() == 1)
	{
		channels = command.get_cmd_params()[0];
		if (channels == "0")
			part_from_all_channels(user);
	}
	else if (command.get_cmd_params().size() == 2)
	{
		channels = command.get_cmd_params()[0];
		std::string keys = command.get_cmd_params()[1];
		size_t channel_index = 0;
		size_t key_index = 0;
		int prev_key_index = 0;
		int prev_chan_index = 0;
		std::string channel_name;
		std::string key;
		while (((key_index = keys.find(',', key_index)) != std::string::npos || prev_key_index < keys.length()) && ((channel_index = channels.find(',', channel_index)) != std::string::npos || prev_chan_index < channels.length()))
		{
			channel_index == std::string::npos ? channel_index = channels.length() : 0;
			key_index == std::string::npos ? key_index = keys.length() : 0;
			channel_name = channels.substr(prev_chan_index, channel_index - prev_chan_index);
			key = keys.substr(prev_key_index, key_index - prev_key_index);
			if (find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1)) == __channels.end())
			{
				add_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1), key);
				chan = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
				// (*chan).set_topic("No topic is set");
				(*chan).add_operator(user);
				(*chan).add_user(&user);
				(*chan).set_password(key);
				user.add_channel(&(*chan));
				user.set_channel_op(true);
				// std::cout << "list of users in channel " << (*chan).get_name() << std::endl;
				// for (std::list<User *>::iterator it = (*chan).get_users().begin(); it != (*chan).get_users().end(); it++)
				// {
				// 	std::cout << (*it)->get_nickname() << std::endl;
				// }
				// std::cout << "list of operators in channel " << (*chan).get_name() << std::endl;
				// for (std::vector<User >::iterator it = (*chan).get_operators().begin(); it != (*chan).get_operators().end(); it++)
				// {
				// 	std::cout << (it)->get_nickname() << std::endl;
				// }
			}
			else
			{
				chan = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1)); /*this line is here for the print test*/
				if (find_user_in_channel(user, *chan) != *(*chan).get_users().end())
				{
					std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 443 " + user.get_nickname() + " " + channel_name  + " :is already on channel\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
					return ;
				}
				else
				{
					std::list<Channel>::iterator cho = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
					if (find_user_in_channel(user, *cho) == *(*cho).get_users().end())
					{
						if ((*cho).get_password() == key)
						{
							if ((*cho).is_user_banned(user))
								write_reply(user, ERR_BANNEDFROMCHAN, command);
							else if ((*cho).get_modes().get_i())
								write_reply(user, ERR_INVITEONLYCHAN, command);
							else
							{							
								std::string	full_msg = ":" + this->__name + " " + command.get_cmd() + " 332 " + command.get_cmd_params()[0] + " :" + (*chan).get_topic() + " " + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
								send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
								for (std::list<User *>::iterator it = (*cho).get_users().begin(); it != (*cho).get_users().end(); it++)
								{
									std::string full_msg = user.full_id() + " JOIN " + channel_name + "\n";
									write_socket((*it)->get_fd() , full_msg);
								}
								(*cho).add_user(&user);
								user.add_channel((&(*chan)));
								write_reply(user, RPL_NAMREPLY, command); //uncomment later when it is implemented
								write_reply(user, RPL_ENDOFNAMES, command);
							}
						}
						else
						{
							write_reply(user, ERR_BADCHANNELKEY, command);
						}
					}
				}
			}
			key_index++;
			prev_key_index = key_index;
			channel_index++;
			prev_chan_index = channel_index;
		}
	}
	else if (command.get_cmd_params().size() > 2)
		write_socket(user.get_fd(), "more args than necessary\n");
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
	// std::cout << "List of users after join :" << std::endl;
	// for (std::list<User *>::iterator it = (*chan).get_users().begin() ; it != (*chan).get_users().end() ; it++)
	// {
	// 	std::cout << (*it)->get_nickname() << std::endl;
	// }
	// std::cout << "channel names list" << std::endl;
	// for (std::list<Channel>::iterator it = get_channels().begin(); it != get_channels().end(); it++)
	// {
	// 	std::cout << "|" << (*it).get_prefix() << (*it).get_name() << "| pass = |" << (*it).get_password() << "|" << std::endl;
	// }
}
