#include "msg_parse.hpp"
#include "Server.hpp"

int	Server::is_real_user(std::string nickname)
{
	for (std::list<User>::iterator it = __users.begin() ; it != __users.end(); it++)
	{
		if ((*it).get_nickname() == nickname)
			return (1);
	}
	return (0);
}

User *Server::find_user_in_channel_by_nick(std::string nickname, Channel chan)
{
	std::list<User *>::iterator it = chan.get_users().begin();
	for ( ; it != chan.get_users().end() ; it++)
	{
		if ((*it)->get_nickname() == nickname)
			return (*it);
	}
	return (*it);
}

int		Server::is_operator_on_channel(User user, Channel chan)
{
	for (std::list<User *>::iterator it = chan.get_users().begin(); it != chan.get_users().end(); it++)
	{
		if ((*it)->get_nickname() == user.get_nickname())
			return (1);
	}
	return (0);
}

void	Channel::add_to_invited_list(User &user)
{
	__invited_list.push_back(user);
}

std::vector<User> &Channel::get_invited_list(void)
{
	return (__invited_list);
}

void	Server::INVITE_handler(User &user, msg_parse &command)
{
	if (command.get_cmd_params().size() > 1)
	{
		if (command.get_cmd_params().size() > 2)
		{
			write_socket(user.get_fd(), user.full_id() + ": more params than necessary\n");
			return ;
		}
		std::string channel_without_prefix = command.get_cmd_params()[1];
		Channel chan;
		if (find_channel(command.get_cmd_params()[1][0], channel_without_prefix.substr(1, channel_without_prefix.length() - 1)) == get_channels().end())
		{
			write_reply(user, ERR_NOSUCHCHANNEL, command);
			return ;
		}
		chan = *find_channel(command.get_cmd_params()[1][0], channel_without_prefix.substr(1, channel_without_prefix.length() - 1));
		if (find_user_in_channel(user, chan) != *chan.get_users().end())
		{
			write_reply(user, ERR_NOSUCHCHANNEL, command);
			return ;
		}
		if (is_real_user(command.get_cmd_params()[0]))
		{
			write_reply(user, ERR_NOSUCHNICK, command);
			return ;
		}
		if (find_user_in_channel_by_nick(command.get_cmd_params()[0], chan) != *chan.get_users().end())
		{
			write_reply(user, ERR_USERONCHANNEL, command);
			return ;
		}
		User invited_user = *find_user_in_channel_by_nick(command.get_cmd_params()[0], chan);
		if (invited_user.get_modes().get_a())
		{
			write_reply(user, RPL_AWAY, command);
			return ;
		}
		if (chan.get_modes().get_i() && !is_operator_on_channel(user, chan))
		{
			write_reply(user, ERR_CHANOPRIVSNEEDED, command);
			return ;
		}
		chan.add_to_invited_list(invited_user);
		/*Write reply message to both inviter and invited <Channel> <nick>*/
	}
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
}