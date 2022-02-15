#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include "User.hpp"
#include "Server.hpp"
#include "CModes.hpp"
#include <string>
#include <list>
#include <vector>
#include <set>

class User;

class Channel
{
private:
	// *	&: Local channel, on ly on this server
	// *	+: Do not support channel modes
	// *	!: Creator marked as CHannel OPerator
	// *	#: other ? normal channel ?
	char					__prefix;
	std::string				__name; //
	std::list<User *>		__users;
	CModes					__modes;
	std::string				__topic;
	std::string				__password;
	int						__size;
	std::list<std::string>	__banned;
	std::set<std::string>	__operators;
	// may need a channel mask

public:
	// * Copliens form
	Channel();
	Channel(const Channel &rhs);
	Channel(char prefix, std::string name, std::string topic = "", int size = -1);
	Channel(std::string full_name, std::string password = "");
	~Channel();
	Channel			&operator=(const Channel &rhs);

	bool				operator==(const Channel &rhs);
	bool				operator!=(const Channel &rhs);
	char				get_prefix() const;
	std::set<std::string>	get_operators( void) ; //added
	std::string			get_name() const;
	std::list<User *>	&get_users();
	std::string			get_topic() const;
	CModes				&get_modes();
	void				set_topic(const std::string topic);
	int					add_user(User *u);
	int					remove_user(User *u);
	bool				has_user(User *u);
	bool				is_user_banned(const User &u);
	void				set_password(std::string pass);
	void				add_operator(std::string);
	void				ban_mask(std::string str);
	std::string			str_name() const;
	void				send_message(Server &serv, User &u, std::vector<std::string> message) const;
	void				send(Server &serv, std::string message);

};

#endif