#ifndef __USER_HPP__
#define __USER_HPP__

#include "Channel.hpp"
#include "UModes.hpp"
#include <string>
#include <list>
#include <vector>

class Channel;

# define MAX_CHANNELS 5

class User
{
	private:
		int						__fd;
		bool					__real_user; // bot
		int						__pass_checked; //added
		bool					__is_channel_op;
		// bool					__is_away; //added
		std::string				__nick_name; // maximum length of nine
		std::string				__user_name;
		std::string				__host_name;
		std::string				__real_name; // full name
		UModes					__modes;
		std::string				__away_msg;
		std::list<Channel *>		__channels;
		std::vector<std::string> __msgs;  // added

	public:
		// * Copliens form
		User();
		User(const int &fd, const std::string &nick = "", const std::string &name = "");
		User(const User &rhs);
		~User();

		User	&operator=(const User &rhs);
		bool	operator==(const User &rhs) const;
		bool	operator!=(const User &rhs) const;

		int			get_fd() const;
		std::string	get_nickname() const;
		std::string	get_username() const;
		std::string	get_hostname() const;
		std::string	get_realname() const;
		UModes		&get_modes();
		std::string	get_away_msg() const;
		std::vector<std::string> &get_msgs(void);
		int			get_nb_channels() const;
		// bool		get_is_away() const;
		int			get_pass_check( void) const;// added
		std::list<Channel *>	&get_channels(); //added
		bool		is_real_user() const;
		bool		is_channel_op() const;

		void		set_fd(int fd);
		void		set_nickname(std::string nickname);
		void		set_username(std::string username);
		void		set_hostname(std::string hostname);
		void		set_realname(std::string realname);
		void		set_modes(char m);
		void		unset_modes(char m);
		void		set_away_msg(std::string away_msg);
		void		set_is_real_user(bool s);
		void		set_pass_check(int num); //added
		void		set_channel_op(bool val);
		// void		set_is_away(bool); //added

		bool		matches_mask(std::string mask) const;
		int			add_channel(Channel *c);
		int			remove_channel(Channel *c);

		std::string	full_id() const;

};

// template <class user>
// bool operator<(const User &lhs, const User &rhs)
// {
// 	return (lhs.get_fd() < rhs.get_fd());
// }

#endif