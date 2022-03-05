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
#include "msg_parse.hpp"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <fstream>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#define RPL_WELCOME 001
#define RPL_MYINFO 004
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
// #define RPL_NOTOPIC 331
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341 //reply not implemented yet
#define RPL_YOUREOPER 381
#define ERR_NOSUCHCHANNEL 403
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_CHANOPRIVSNEEDED 482
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
#define RPL_MOTDSTART 375
#define RPL_MOTD 372
#define RPL_ENDOFMOTD 376
#define ERR_NOMOTD 422
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_LIST 322
#define RPL_LISTEND 323
#define ERR_NOCHANMODES 477
#define ERR_UNKNOWNMODE 472
#define ERR_KEYSET 467
#define ERR_CANNOTSENDTOCHAN 404
#define RPL_CHANNELMODEIS 324
// #define ERR_NOSUCHCHANNEL 503


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
		int												get_status() const;
		int												get_nbr_of_unknown_conns() const;
		std::string										get_name() const;
		int												get_port() const;
		std::string										get_password() const;
		int												get_serverfd() const;
		std::list<User>									get_users();
		std::string										get_motd() const;
		std::list<Channel>								&get_channels();

		// * Setters
		void											set_name(std::string name);
		void											set_port(int port);
		void											set_password(std::string password);
		void											set_motd(std::string motd);
		void											inc_nbr_of_unknown_conns(void);
		void											dec_nbr_of_unknown_conns(void);
		// Server managment
		int												create_socket();
		int												set_socket();
		int												bind_socket();
		int												listen_socket();

		int 											init_server();
		int												start_server();

		void											reset_sockets();
		int												accept_connection();
		int												read_socket(User &u);
		int												write_socket(int const fd, std::string const message) const;
		int												receive_data();
		int												check_connection();

		// * Channel managment

		User											*getuserbynick(const std::string &nick);
		User											*getuserbyfd(int fd);
		int												disconnect_user(const std::list<User>::iterator &it);
		int												disconnect_user(const User &u);
		std::pair<std::list<Channel>::iterator, bool>	add_channel(char prefix, std::string name, std::string password);
		int 											delete_channel(std::string name);
		std::list<Channel>::iterator					has_channel(std::string full_name);
		std::list<Channel>::iterator					find_channel(char prefix, std::string name);
		// Command management
		int												write_reply(User &user, int reply_code, msg_parse &command);
		int												check_for_bad_char(char *nickname);
		int												PRIVMSG_handler(msg_parse &command, User &user);
		int												AWAY_handler(msg_parse &command, User &user);
		int												MODE_handler(msg_parse &command, User &user);
		int												send_available_commands(User &user);
		int												user_mode_setter(msg_parse &command, User &user);
		int												check_syntax(msg_parse &command);
		int												is_real_user(std::string nickname);
		int												is_operator_on_channel(User &user, Channel &chan);
		void											WHOIS_handler(msg_parse &command, User &user);
		void											LUSERS_handler(msg_parse &command, User &user);
		void											QUIT_handler(User &user);
		void											OPER_handler(User &user, msg_parse &command);
		void											TOPIC_handler(User &user, msg_parse &command);
		void											JOIN_handler(User &user, msg_parse &command);
		void											PART_handler(User &user, msg_parse &command);
		void											INVITE_handler(User &user, msg_parse &command);
		void											send_msg_to_channel_users(Channel &chan, std::string &message);
		void											part_from_all_channels(User &user);
		void											MOTD_handler(msg_parse &command, User &user);
		void											NAMES_handler(msg_parse &command, User &user);
		void											LIST_handler(msg_parse &command, User &user);
		void											KICK_handler(User &user, msg_parse &command);
		void											as_many(User &user, msg_parse &command, int &check_is_op);
		void											one_chan(User &user, msg_parse &command, int &check_is_op);
		void											remove_user(User &user);
		void											CHANNEL_MODE_handler(msg_parse &command, User &user);
		void											add_remove_mode_to_channel(Channel &channel, msg_parse &command, User &user, char c);
		void											check_command(msg_parse &command, User &user);
		void											user_authentication( msg_parse &command, User &user);
		User											&find_user_in_server(std::string &nickname);
		User											*find_user_in_channel(User &user, Channel &channel);
		User 											*find_user_in_channel_by_nick(std::string nickname, Channel &chan);
		int												parse_and_execute(char *buffer, int ret, msg_parse &parsed_command,User &user);

};

#endif