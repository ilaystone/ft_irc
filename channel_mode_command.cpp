#include "Server.hpp"

int	is_mode_valid(char c)
{
	int	pos;
	std::string modes = "ovaimnqpsrtkl";

	if ((pos = modes.find(c)) == std::string::npos)
		return -1;
	return (pos);
}

void	Channel::apply_mode(int pos, User &user, msg_parse &command)
{
	bool result;

	switch (pos)
	{
		case 0: // mode o
			this->is_operator(user.get_nickname()) == false ? this->add_operator(user) : (void)0;
			break;
		case 1: // mode v
			this->has_voice(user.get_nickname()) == false ? this->add_voice_privilege(user) : (void)0;
			break;
		case 2: // mode a
			this->get_modes().set_a(true);
			break;
		case 3: // mode i
			this->get_modes().set_i(true);
			break;
		case 4: // mode m
			this->get_modes().set_m(true);
			break;
		case 5: // mode n
			this->get_modes().set_n(true);
			break;
		case 6: // mode q
			this->get_modes().set_q(true);
			break;
		case 7: // mode p
			result = (this->get_modes().get_s() == false) ? true : false;
			this->get_modes().set_p(result);
			break;
		case 8: // mode s
			result = (this->get_modes().get_p() == false) ? true : false;
			this->get_modes().set_s(result);
			break;
		case 9: // mode r
			this->get_modes().set_r(true);
			break;
		case 10: // mode t
			this->get_modes().set_t(true);
			break;
		case 11: // mode k
			this->get_modes().set_k(true);
			this->set_password(command.get_cmd_params()[2]);
			break;
		case 12: // mode l
			this->get_modes().set_l(true);
			this->set_size(std::stoi(command.get_cmd_params()[2]));
			break;
		
		default:
			break;
	}
}

void	Channel::remove_mode(int pos, User &user)
{
	switch (pos)
	{
		case 0:
			this->remove_operator(user);
			break;
		case 1:
			this->remove_voice_privilege(user);
			break;
		case 2:
			this->get_modes().set_a(false);
			break;
		case 3:
			this->get_modes().set_i(false);
			break;
		case 4:
			this->get_modes().set_m(false);
			break;
		case 5:
			this->get_modes().set_n(false);
			break;
		case 6:
			this->get_modes().set_q(false);
			break;
		case 7:
			this->get_modes().set_p(false);
			break;
		case 8:
			this->get_modes().set_s(false);
			break;
		case 9:
			this->get_modes().set_r(false);
			break;
		case 10:
			this->get_modes().set_t(false);
			break;
		case 11:
			this->get_modes().set_k(false);
			this->set_password("");
			break;
		case 12:
			this->get_modes().set_l(false);
			this->set_size(-1);
			break;
		
		default:
			break;
	}
}

bool	check_o_v_k_l_modes(int &pos, std::string modes, User &user, char c)
{
	if (c == '+')
	{
		if (pos == 0) // Mode +o
			if (modes.find("l") != std::string::npos || modes.find("k") != std::string::npos)
				return false;
		if (pos == 1) // Mode +v
			if (modes.find("l") != std::string::npos || modes.find("k") != std::string::npos)
				return false;
		if (pos == 11) // Mode +k
			if (modes.find("o") != std::string::npos || modes.find("v") != std::string::npos || modes.find("l") != std::string::npos)
				return false;
		if (pos == 12) // Mode +l
			if (modes.find("o") != std::string::npos || modes.find("v") != std::string::npos || modes.find("k") != std::string::npos)
				return false;
	}
	return true;
}

void	Server::add_remove_mode_to_channel(Channel &channel, msg_parse &command, User &user, char c)
{
	int	pos;
	std::string mode = command.get_cmd_params()[1] + 1;
	User *tmp;

	for (int i = 0; i < mode.size(); i++)
	{
		command.set_pos(i + 1);
		if ((pos = is_mode_valid(mode[i])) == -1)
		{
			write_reply(user, ERR_UNKNOWNMODE, command);
		}
		else
		{
			if ((pos == 0 || pos == 1 || pos == 11 || pos == 12) && command.get_cmd_params().size() < 3)
				write_reply(user, ERR_NEEDMOREPARAMS, command);
			else if (command.get_cmd_params().size() >= 2)
			{
				if (check_o_v_k_l_modes(pos, command.get_cmd_params()[1], user, c)) // Checking if there's any combination of o,k,v,l except ov. 
				{
					if (pos == 0 || pos == 1) //mode is either +o or +v
					{
						tmp = getuserbynick(command.get_cmd_params()[2]);
						if (tmp != nullptr)
						{
							if (find_user_in_channel(*tmp, channel) == *(channel.get_users().end()))
								write_reply(user, ERR_USERNOTINCHANNEL, command);
							else
							{
								c == '+' ? channel.apply_mode(pos, *tmp, command) : channel.remove_mode(pos, *tmp);
								for (std::list<User *>::iterator it = channel.get_users().begin(); it != channel.get_users().end(); it++)
									write_reply(*(*it), RPL_CHANNELMODEIS, command);
							}
						}
						else
							write_reply(user, ERR_USERNOTINCHANNEL, command);
					}
					else
					{
						if (pos == 11 && channel.get_modes().get_k() == true)
							write_reply(user , ERR_KEYSET, command);
						else
						{
							c == '+' ? channel.apply_mode(pos, *tmp, command) : channel.remove_mode(pos, *tmp);
							for (std::list<User *>::iterator it = channel.get_users().begin(); it != channel.get_users().end(); it++)
								write_reply(*(*it), RPL_CHANNELMODEIS, command);
						}
					}
				}
				else
				{
					send(user.get_fd(), "Usage : If the mode takes a parameter, you can only use {k}, {l} or {o and/or v}\n", 82, 0);
					break ;
				}
			}
			else
				write_reply(user, ERR_NEEDMOREPARAMS, command);
		}
	}
}

void	Server::CHANNEL_MODE_handler(msg_parse &command, User &user)
{
	std::list<Channel>::iterator it;
	std::string name;

	if (command.get_cmd_params().size() < 2)
		write_reply(user, ERR_NEEDMOREPARAMS, command);
	else
	{
		name = command.get_cmd_params().front();
		if ((it = find_channel(name[0], name.substr(1, name.length() - 1))) != this->__channels.end())
		{
			if ((*it).is_operator(user.get_nickname()) == true || user.get_modes().get_o() == true)
			{
				if (command.get_cmd_params()[1][0] == '+')
					add_remove_mode_to_channel(*it, command, user, '+');
				else if (command.get_cmd_params()[1][0] == '-')
					add_remove_mode_to_channel(*it, command, user, '-');
			}
			else
				write_reply(user, ERR_CHANOPRIVSNEEDED, command);
		}

	}
}