#ifndef __SERVER_HPP__
#define __SERVER_HPP__

class Server;
class User;
class Channel;

#define TRUE 1
#define FALSE 0

#include "User.hpp"
#include "Channel.hpp"
#include "Commands.hpp"
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include "msg_parse.hpp"
#include <set>
#include	<string>  

#define RPL_WELCOME 001
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
#define RPL_TOPIC 332
#define RPL_NOTOPIC 332
#define RPL_YOUREOPER 381
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTONCHANNEL 442
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_NOSUCHNICK 401
#define ERR_TOOMANYTARGETS 407
#define RPL_AWAY 301
#define ERR_UNKNOWNCOMMAND 421
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_ENDOFWHOIS 318
#define RPL_LUSERCLIENT 251
#define RPL_LUSEROP 252
#define RPL_LUSERUNKNOWN 253
#define RPL_LUSERCHANNELS 254
#define RPL_LUSERME 255
#define RPL_JOINED 503


void	print_command(msg_parse &command);
class Server
{
	private:
		int						__status;
		int						__nbr_of_unknown_conns;
		std::string				__name;
		int						__port;
		std::string				__password;
		int						__serverfd;
		std::list<User>			__users;
		fd_set					__sockets;
		std::string				__motd;
		std::list<Channel>		__channels;
		std::set<std::string>	__list_nicks;

		// * Copliens form
		Server(const Server &rhs);
		Server	&operator=(const Server &rhs);
	public:
		// * Copliens form
		Server();
		Server(int port, std::string password, std::string name = "");
		~Server();

		// * Getters
		int	get_status() const;
		int	get_nbr_of_unknown_conns() const;
		std::string	get_name() const;
		int	get_port() const;
		std::string	get_password() const;
		int	get_serverfd() const;
		std::list<User>	get_users();
		std::string	get_motd() const;
		std::list<Channel>	&get_channels();

		// * Setters
		void	set_name(std::string name);
		void	set_port(int port);
		void	set_password(std::string password);
		void	set_motd(std::string motd);
		void	inc_nbr_of_unknown_conns(void);
		void	dec_nbr_of_unknown_conns(void);

		int								create_socket();
		int								set_socket();
		int								bind_socket();
		int								listen_socket();

		int 							init_server();
		int								start_server();

		void							reset_sockets();
		int								accept_connection();
		// int								message_splitter(char *&buffer);
		int								read_socket(User &u);
		int								write_socket(int const fd, std::string const message) const;
		int								receive_data();
		int								check_connection();

		// * Utility

		User							*getuserbynick(const std::string &nick);
		User							*getuserbyfd(int fd);
		int								disconnect_user(const std::list<User>::iterator &it);
		int								disconnect_user(const User &u);
		std::list<Channel>::iterator		add_channel(char prefix, std::string name, std::string password);
		int 							delete_channel(std::string name);
		std::list<Channel>::iterator		has_channel(std::string full_name);
		std::list<Channel>::iterator		find_channel(char prefix, std::string name);
		// start commands
		void							check_command(msg_parse &command, User &user);
		void							user_authentication( msg_parse &command, User &user);
		int								write_reply(User &user, int reply_code, msg_parse &command);
		int								check_for_bad_char(char *nickname);
		int								PRIVMSG_handler(msg_parse &command, User &user);
		int								AWAY_handler(msg_parse &command, User &user);
		int								MODE_handler(msg_parse &command, User &user);
		void							WHOIS_handler(msg_parse &command, User &user);
		void							LUSERS_handler(msg_parse &command, User &user);
		int								user_mode_setter(msg_parse &command, User &user);
		void							QUIT_handler(User &user, msg_parse &command);
		void							OPER_handler(User &user, msg_parse &command);
		void							TOPIC_handler(User &user, msg_parse &command);
		void							JOIN_handler(User &user, msg_parse &command);
		User							*find_user_in_channel(User user, Channel &channel);
};

#endif