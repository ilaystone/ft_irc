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

class Server
{
	private:
		int						__status;
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
		std::string	get_name() const;
		int	get_port() const;
		std::string	get_password() const;
		int	get_serverfd() const;
		std::list<User>	get_users();
		std::string	get_motd() const;
		std::list<Channel>	get_channels();

		// * Setters
		void	set_name(std::string name);
		void	set_port(int port);
		void	set_password(std::string password);
		void	set_motd(std::string motd);

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
		std::list<Channel>::iterator	add_channel(std::string name, std::string password);
		int 							delete_channel(std::string name);
		std::list<Channel>::iterator	has_channel(std::string full_name);
		std::list<Channel>::iterator	find_channel(char prefix, std::string name);
		// start commands
		void							check_command(msg_parse command, User &user);
		void							user_authentication( msg_parse command, User &user);
	
};

#endif