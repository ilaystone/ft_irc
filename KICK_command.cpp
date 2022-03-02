#include "Server.hpp"
#include "msg_parse.hpp"

int		Server::check_syntax(msg_parse &command)
{
	size_t index = 0;
	size_t prev_index = 0;
	size_t	pos = 0;
	int c_index = 0;
	int u_index = 0;
	std::string channels = command.get_cmd_params()[0];
	std::string users = command.get_cmd_params()[1];
	while ((pos = channels.find(',', prev_index)) != std::string::npos)
	{
		c_index++;
		index = pos + 1;
		prev_index = index;
	}
	pos = 0;
	prev_index = 0;
	while ((pos = users.find(',', prev_index)) != std::string::npos)
	{
		u_index++;
		index = pos + 1;
		prev_index = index;
	}
	if (c_index != u_index && c_index > 0)
		return (0);
	if (c_index == u_index)
		return (2);
	else if (c_index == 0)
		return (3);
	return(1) ;
}

void	Server::as_many(User &user, msg_parse &command, int &check_is_op)
{
	std::string channels = command.get_cmd_params()[0];
	std::string users = command.get_cmd_params()[1];
	size_t channel_index = 0;
	size_t user_index = 0;
	int prev_user_index = 0;
	int prev_chan_index = 0;
	std::string channel_name;
	std::string user_;
	std::string	full_msg;
	while (((channel_index = channels.find(',', channel_index)) != std::string::npos || prev_chan_index < channels.length()) && ((user_index = users.find(',', user_index)) != std::string::npos || prev_user_index < users.length()))
	{
		channel_index == std::string::npos ? channel_index = channels.length() : 0;
		user_index == std::string::npos ? user_index = users.length() : 0;
		channel_name = channels.substr(prev_chan_index, channel_index - prev_chan_index);
		user_ = users.substr(prev_user_index, user_index - prev_user_index);
		if (find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1)) == __channels.end())
		{
			full_msg = ":" + this->__name + " " + command.get_cmd_params()[0] + " 403 :No such channel\n"/* + user.full_id() + " "*/;
			send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
		}
		else
		{
			Channel chan = *find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1));
			if (find_user_in_channel(user, chan) == *chan.get_users().end() && check_is_op != 2)
			{
				full_msg = ":" + this->__name + " " + command.get_cmd() + " 442 " + chan.get_name() + " :You're not on that channel\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
			else if (find_user_in_channel_by_nick(user_, chan) == *chan.get_users().end() && check_is_op != 2)
			{
				full_msg = ":" + this->__name + " " + command.get_cmd() + " 441 " + user_ + " " + chan.get_name() + " :They aren't on that channel\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
			else if (!is_operator_on_channel(user, chan) && check_is_op != 2 && !user.get_modes().get_o())
			{
				full_msg = ":" + this->__name + " " + command.get_cmd() + " 482 " + chan.get_name() + " :You're not channel operator\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
				send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
			}
			else
			{
				// std::cout << " " << chan.get_users().size() << std::endl;
				// for (std::list<User *>::iterator it = chan.get_users().begin() ; it != chan.get_users().end(); it++)
				// {
				// 	std::cout << (*it)->get_nickname();
				// }
				std::list<Channel>::iterator chan_it = find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1));
				User *kicked_user = find_user_in_channel_by_nick(user_, chan);
				full_msg = "PART " + channel_name;
				msg_parse parting(full_msg);
				parting.parser();
				if (command.get_additional_param().size())
					write_socket((*kicked_user).get_fd(), command.get_additional_param());
				else if (command.get_cmd_params().size() == 3)
					write_socket((*kicked_user).get_fd(), command.get_cmd_params()[2]);
				else
					write_socket((*kicked_user).get_fd(), "byeeeee");
				PART_handler((*kicked_user), parting);
				(*chan_it).ban_user(*kicked_user);
				check_is_op == 1 ? check_is_op = 2 : 0;
				// for (std::list<User *>::iterator it = chan.get_users().begin() ; it != chan.get_users().end(); it++)
				// {
				// 	std::cout << (*it)->get_nickname();
				// }
			}
		}
		user_index++;
		prev_user_index = user_index;
		channel_index++;
		prev_chan_index = channel_index;
	}
}

void	Server::one_chan(User &user, msg_parse &command, int &check_is_op)
{
	std::string channel = command.get_cmd_params()[0];
	std::string users = command.get_cmd_params()[1];
	size_t user_index = 0;
	size_t prev_user_index = 0;
	std::string user_;
	while ((user_index = users.find(',', user_index)) != std::string::npos || prev_user_index < users.length())
	{
		user_index == std::string::npos ? user_index = users.length() : 0;
		user_ = users.substr(prev_user_index, user_index - prev_user_index);
		std::string buff = "PART " + channel + " " + user_; 
		msg_parse parting_command(buff);
		parting_command.parser();
		as_many(user, parting_command, check_is_op);
		user_index++;
		prev_user_index = user_index;
	}
}

void	Server::KICK_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() >= 2)
	{
		int check = 0;
		int ret = 0;
		ret = check_syntax(command);
		if (!ret)
			write_socket(user.get_fd() , "wrong syntax !");
		else if (ret == 2)
			as_many(user, command, check);
		else if (ret == 3)
		{
			check = 1;
			one_chan(user, command, check);
		}
	}
	// else if (command.get_cmd_params().size() > 3)
	// 	write_socket(user.get_fd(), "more args than necessary\n");
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}