#include "User.hpp"

User::User()
:	__fd(-1),
	__nick_name(""),
	__user_name(""),
	__is_channel_op(false)
{
	__pass_checked = FALSE;
	return ;
}

User::User(const int &fd, const std::string &nick, const std::string &name)
:	__fd(fd),
	__nick_name(nick),
	__user_name(name),
	__is_channel_op(false)
{
	__pass_checked = FALSE;
	return ;
}

User::User(const User &rhs)
{
	*this = rhs;
}

User	&User::operator=(const User &rhs)
{
	this->__fd = rhs.__fd;
	this->__nick_name = rhs.__nick_name;
	this->__user_name = rhs.__user_name;
	this->__host_name = rhs.__host_name;
	this->__pass_checked = rhs.__pass_checked;
	return *this;
}

User::~User()
{
	return ;
}

bool	User::operator==(const User &rhs) const
{
	return (this->__fd == rhs.__fd && this->__nick_name == rhs.__nick_name && this->__user_name == rhs.__user_name &&
			this->__host_name == rhs.__host_name && this->__real_name == rhs.__real_name && this->__pass_checked == rhs.__pass_checked);
}

bool	User::operator!=(const User &rhs) const
{
	return !(*this == rhs);
}

std::list<Channel* >	&User::get_channels() 
{
	return this->__channels;
}


int		User::get_fd() const
{
	return this->__fd;
}

std::string	User::get_nickname() const
{
	return this->__nick_name;
}

std::string User::get_username() const
{
	return this->__user_name;
}

std::string	User::get_hostname() const
{
	return this->__host_name;
}

std::string	User::get_realname() const
{
	return this->__real_name;
}

UModes	&User::get_modes()
{
	return this->__modes;
}

std::string	User::get_away_msg() const
{
	return this->__away_msg;
}

std::vector<std::string>	&User::get_msgs()
{
	return this->__msgs;
}

int		User::get_nb_channels() const
{
	return this->__channels.size();
}

int		User::get_pass_check( void) const
{
	return (__pass_checked);
}

bool	User::is_real_user( void) const
{
	return this->__real_user;
}

bool	User::is_channel_op( void) const
{
	return this->__is_channel_op;
}

void	User::set_fd(int fd)
{
	this->__fd = fd;
}

void	User::set_nickname(std::string nickname)
{
	this->__nick_name = nickname;
}

void	User::set_username(std::string username)
{
	this->__user_name = username;
}

void	User::set_hostname(std::string hostname)
{
	this->__host_name = hostname;
}

void	User::set_realname(std::string realname)
{
	this->__real_name = realname;
}

void	User::set_modes(char m)
{
	this->__modes.set(m);
}

void		User::unset_modes(char m)
{
	this->__modes.unset(m);
}

void	User::set_away_msg(std::string away_msg)
{
	this->__away_msg = away_msg;
}

void	User::set_is_real_user(bool s)
{
	this->__real_user = s;
}

void		User::set_pass_check(int num)
{
	this->__pass_checked = num;
}

void		User::set_channel_op(bool val)
{
	this->__is_channel_op = val;
}

/** 
 * 	Wild Card Emulation
 */
bool	matches_expression(std::string str, std::string match)
{
	int		str_i = 0;
	int		match_i = 0;

	if (match == "*")
		return true;
	while (match[match_i])
	{
		if (match[match_i] == '?')
		{
			if (str[str_i] == '\0')
				return false;
			match_i++;
			str_i++;
		}
		else if (match[match_i] == '*')
		{
			if (matches_expression(str.substr(str_i), match.substr(match_i + 1)))
				return (true);
			if (str[str_i] == '\0' && matches_expression(str.substr(str_i + 1), match.substr(match_i)))
				return true;
			return false;
		}
		else
		{
			if (std::tolower(str[str_i]) != std::tolower(match[match_i]))
				return false;
			match_i++;
			str_i++;
		}
	}
	return (str[str_i] == '\0' && match[match_i] == 0);
}

bool	User::matches_mask(std::string mask) const
{
	std::string	nick, user, host;
	int		i = 0;
	int		first = 0;
	while (mask[i] != '!')
		i++;
	nick = mask.substr(first, i);
	++i;
	first = i;
	while (mask[i] != '@')
		i++;
	user = mask.substr(first, i);
	++i;
	first = i;
	while (mask[i] != '\0')
		i++;
	host = mask.substr(first, i);
	return (matches_expression(this->__nick_name, nick) &&
			matches_expression(this->__user_name, user) &&
			matches_expression(this->__host_name, host));
}

int		User::add_channel(Channel *u)
{
	if (this->__channels.size() < MAX_CHANNELS)
		this->__channels.push_back(u);
	else
		return (-1);
		// throw std::length_error("Error: User has exceeded channel limit");
	return this->__channels.size();
}

int		User::remove_channel(Channel *u)
{
	std::list<Channel *>::iterator	begin;

	for (begin = this->__channels.begin(); begin != this->__channels.end() && *begin != u; begin++) ;
	if (begin == this->__channels.end())
		return -1;
	this->__channels.erase(begin);
	return this->__channels.size();
}

std::string	User::full_id() const
{
	std::string		res = "";

	res += this->__nick_name + "!" + this->__user_name + "@" + this->__host_name;
	return res;
}
