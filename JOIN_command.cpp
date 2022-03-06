#include "msg_parse.hpp"
#include "Server.hpp"

void		Server::part_from_all_channels(User &user)
{
	while (user.get_channels().size())
	{
		Channel *chan = user.get_channels().front();
		std::string name = chan->get_prefix() + chan->get_name();
		std::string buff = "PART " + name;
		msg_parse command(buff);
		command.parser();
		PART_handler(user, command);
	}
	
}

bool		Channel::is_invited(User &user)
{
	std::vector<User *>::iterator it = __invited_list.begin();

	for (; it != __invited_list.end(); it++)
	{
 		if ((*it)->get_nickname() == user.get_nickname())
			return (1);
	}
	return (0);
}

void		Server::JOIN_handler(User &user, msg_parse &command)
{
	std::list<Channel>::iterator chan;
	std::string channels;

	if (command.get_cmd_params().size() >= 1)
	{
		channels = command.get_cmd_params()[0];
		if (command.get_cmd_params().size() == 1 && channels == "0")
		{
			part_from_all_channels(user);
			return ;
		}
		std::string keys;
		keys = command.get_cmd_params().size() >= 2 ? command.get_cmd_params()[1] : "";
		size_t	channel_index = 0;
		size_t	key_index = 0;
		int		print = 0;
		size_t	prev_key_index = 0;
		size_t	prev_chan_index = 0;
		std::string channel_name;
		std::string key;
		while (((channel_index = channels.find(',', channel_index)) != std::string::npos || prev_chan_index < channels.length()))
		{
			key_index = keys.find(',', key_index);
			channel_index == std::string::npos ? channel_index = channels.length() : 0;
			key_index == std::string::npos ? key_index = keys.length() : 0;
			channel_name = channels.substr(prev_chan_index, channel_index - prev_chan_index);
			key = keys.substr(prev_key_index, key_index - prev_key_index);
			std::string	full_msg;
			if (find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1)) == __channels.end())
			{
				if (!add_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1), key).second)
				{
					std::string full_msg = ":" + this->__name + "JOIN " + ":Erroneous channel name.\n";
					write_socket(user.get_fd(), full_msg);
				}
				else
				{
					chan = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
					if (user.add_channel(&(*chan)) == -1)
					{
						std::string full_msg = ":" + this->__name + "JOIN " + ":You have reached the limit for channels a user can be on.\n";
						write_socket(user.get_fd(), full_msg);
					}
					else
					{
						(*chan).add_operator(user);
						(*chan).add_user(&user);
						(*chan).set_password(key);
						if (key.size() > 0)
							(*chan).get_modes().set_k(true);
						user.set_channel_op(true);
						std::string	full_msg = ":" + user.full_id() + " JOIN " + " :" + (*chan).get_name() + "\n"; 
						send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
						if (print == 0)
						{
							write_reply(user, RPL_NAMREPLY, command);
							write_reply(user, RPL_ENDOFNAMES, command);
						}
					}
				}
			}
			else
			{
				chan = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
				if (find_user_in_channel(user, *chan) != *(*chan).get_users().end())
				{
					full_msg = ":" + this->__name + " " + command.get_cmd() + " 443 " + user.get_nickname() + " " + channel_name  + " :is already on channel :" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"; 
					send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
				}
				else
				{
					std::list<Channel>::iterator cho = find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
					if (cho->get_modes().get_l() && cho->get_users().size() + 1 > cho->get_size())
						write_reply(user, ERR_CHANNELISFULL, command);
					else if (find_user_in_channel(user, *cho) == *(*cho).get_users().end())
					{
						if (((*cho).get_modes().get_k() && (*cho).get_password() == key) || !(*cho).get_modes().get_k())
						{
							if ((*cho).is_user_banned(user) && !user.get_modes().get_o())
								write_reply(user, ERR_BANNEDFROMCHAN, command);
							else if ((*cho).get_modes().get_i() && !(*cho).is_invited(user) && !user.get_modes().get_o())
								write_reply(user, ERR_INVITEONLYCHAN, command);
							else
							{; 
								full_msg = ":" + user.full_id() + " JOIN " + " :" + (*chan).get_name() + "\n"; 
								send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
								if (!(*cho).get_modes().get_q())
								{
									for (std::list<User *>::iterator it = (*cho).get_users().begin(); it != (*cho).get_users().end(); it++)
									{
										std::string full_msg = ":" + user.full_id() + " JOIN " + channel_name + "\n";
										write_socket((*it)->get_fd() , full_msg);
									}
								}
								(*cho).add_user(&user);
								user.add_channel(&(*chan));
								if (print == 0)
								{
									write_reply(user, RPL_NAMREPLY, command);
									write_reply(user, RPL_ENDOFNAMES, command);
								}
							}
						}
						else
							write_reply(user, ERR_BADCHANNELKEY, command);
					}
				}
			}
			key_index < keys.length() ? key_index++ : 0;
			prev_key_index = key_index;
			channel_index++;
			prev_chan_index = channel_index;
		}
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}
