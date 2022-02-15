#include "Server.hpp"
// * private
// * Copliens form

Server::Server(const Server &rhs)
{
	*this = rhs;
}

Server		&Server::operator=(const Server &rhs)
{
	this->__status = rhs.get_status();
	this->__name = rhs.get_name();
	this->__port = rhs.get_port();
	this->__password = rhs.get_password();
	this->__serverfd = get_serverfd();
	this->__users = get_users();
	this->__sockets = rhs.__sockets;
	this->__motd = get_motd();
	return *this;
}

// * public
// * Copliens form

Server::Server()
:	__port(0),
	__password(""),
	__name(""),
	__serverfd(-1),
	__status(0)
{
	this->__users = std::list<User>();
	return ;
}

Server::Server(int port, std::string password, std::string name)
:	__port(port),
	__password(password),
	__name(name),
	__serverfd(-1),
	__status(0)
{
	this->__users = std::list<User>();
	return ;
}

Server::~Server()
{
	return ;
}

// * Getters

int	Server::get_status() const
{
	return this->__status;
}

std::string	Server::get_name() const
{
	return this->__name;
}

int	Server::get_port() const
{
	return this->__port;
}

std::string	Server::get_password() const
{
	return this->__password;
}

int	Server::get_serverfd() const
{
	return this->__serverfd;
}

std::list<User>	Server::get_users() 
{
	return this->__users;
}

std::string	Server::get_motd() const
{
	return this->__motd;
}

std::list<Channel>	&Server::get_channels() 
{
	return this->__channels;
}

// * Setters

void	Server::set_name(std::string name)
{
	this->__name = name;
}

void	Server::set_port(int port)
{
	this->__port = port;
}

void	Server::set_password(std::string password)
{
	this->__password = password;
}

void	Server::set_motd(std::string motd)
{
	this->__motd = motd;
}

int	Server::create_socket()
{
	if ((this->__serverfd = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
	{
		this->__status = -1;
		std::cerr << "Error:\n Socket creation failed with code " << errno << std::endl;
		perror("socket failed");
		return (1);
	}
	this->__status = 1;
	return (0);
}

int	Server::set_socket()
{
	int		opt;

	opt = 1;
	if (setsockopt(this->__serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		this->__status = -1;
		std::cerr << "Error\n setsockpot failed with code " << errno << std::endl;
		perror("setsocketpot(SOL_SOCKET)");
		return (1);
	}
	opt = 0;
	if (setsockopt(this->__serverfd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)))
	{
		this->__status = -2;
		std::cerr << "Error\n setsockpot failed with code " << errno << std::endl;
		perror("setsocketpot(IPPROTO_IPV6)");
		return (1);
	}
	this->__status = 2;
	return 0;
}

int	Server::bind_socket()
{
	struct sockaddr_in6	address;

	address.sin6_family = AF_INET6;
	address.sin6_port = htons(this->__port);
	address.sin6_flowinfo = 0;
	address.sin6_addr = in6addr_any;
	address.sin6_scope_id = 0;
	if (bind(this->__serverfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		this->__status = -3;
		std::cerr << "Error\n bind failed with code " << errno << std::endl;
		perror("bind failed");
		return 1;
	}
	this->__status = 3;
	return 0;
}

int	Server::listen_socket()
{
	if (listen(this->__serverfd, 3) < 0)
	{
		this->__status = -4;
		std::cerr << "Error\n listen failed with code " << errno << std::endl;
		perror("listen");
		return 1;
	}
	this->__status = 4;
	return 0;
}

int	Server::init_server()
{
	if (this->create_socket())
		return this->__status;
	if (this->set_socket())
		return this->__status;
	if (this->bind_socket())
		return this->__status;
	if (this->listen_socket())
		return this->__status;
	return 0;
}

int	Server::start_server()
{
	int		i;
	timeval	timeout;

	if (this->__status != 4)
		std::cerr << "Error\n server is not ready to launch (check server status)" << std::endl;
	else
	{
		this->__status = 5;
		while (this->__status == 5)
		{
			this->reset_sockets();
			timeout.tv_usec = 0;
			timeout.tv_sec = 10;
			i = select(FD_SETSIZE, &(this->__sockets), NULL, NULL, &timeout);
			if (i > 0)
			{
				if (FD_ISSET(this->__serverfd, &(this->__sockets)))
				{
					this->accept_connection();
					// std::cout << "Connection Accepted!" << std::endl;
					i--;
				}
				if (i > 0)
					this->receive_data();
			}
			this->check_connection();
		}
	}
	return 0;
}

void	Server::reset_sockets()
{
	FD_ZERO(&this->__sockets);
	FD_SET(this->__serverfd, &(this->__sockets));
	for (std::list<User>::iterator	begin = this->__users.begin(); begin != this->__users.end(); begin++)
	{
		FD_SET((*begin).get_fd(), &(this->__sockets));
	}
}

int	Server::accept_connection()
{
	int							new_socket;
	struct sockaddr_in6			adress;
	int 						addrlen = sizeof(adress);
	char						buffer[513];
	std::vector<std::string>	message;
	User						new_user;

	bzero(buffer, 513);
	adress.sin6_family = AF_INET6;
	adress.sin6_port = htons(this->__port);
	adress.sin6_flowinfo = 0;
	adress.sin6_addr = in6addr_any;
	adress.sin6_scope_id = 0;

	new_socket = accept(this->__serverfd, (struct sockaddr *)&adress, (socklen_t *)&addrlen);
	if (new_socket < 0)
	{
		perror("accept");
		std::cerr << "Err code " << errno << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Connection Established !" << std::endl;
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		new_user = User(new_socket);
		this->__users.push_back(new_user);
		// std::cerr << "nb_users: " << this->__users.size() << std::endl;
		return 0;
	}
}

// int		Server::message_splitter(char *&buffer)
// {
// 	return 0;
// }

int		Server::read_socket(User &u)
{
	int							read;
	char						buffer[513];
	char						*ptr_buffer = buffer;
	std::vector<std::string>	message;
	int ret = 1;
	msg_parse parsed_command;

	bzero(buffer, 513);
	// std::cout << u.get_pass_check() << std::endl;
	read = recv(u.get_fd(), ptr_buffer, 512, 0);
	// std::cout << "Message received ! read " << read << " Characters \n";
	if (read > 0)
	{
		// std::cerr << "Buffer: " << buffer << std::endl;
		// message.clear();
		message_splitter(ptr_buffer, ret, parsed_command, u);
		ret == 0 ? std::cerr << "Parsing error" << std::endl : 0;
		// print_command(parsed_command);
		// std::cout << u.get_pass_check() << std::endl;
		// if (!message_splitter(ptr_buffer, parsed_command))
		// 	std::cerr << "Parsing error" << std::endl;
		// {
		// 	std::cerr << "raw: |" << buffer << "|\n";
		// 	std::vector<std::string>::iterator begin2, end2;
		// 	std::cerr << "message :\n\t";
		// 	begin2 = message.begin();
		// 	end2 = message.end();
		// 	while (begin2 != end2)
		// 	{
		// 		std::cerr << *begin2 << std::endl;
		// 		begin2++;
		// 	}
		// }
		// std::cout << parsed_command.get_cmd() << std::endl;
		if (!u.get_msgs().size())
			this->check_command(parsed_command, u);
		// this->write_socket(u.get_fd(), "COMMAND RECEIVED");
		// if (u.is_real_user() == true)
		// 	std::cout << buffer;
		// else						// commented
		// 	std::cout << "Server command\n";
		// std::cout << "command terminated with" << read << std::endl;
	}

	return 0; 
}

int		Server::write_socket(const int fd, const std::string message) const
{
	if (send(fd, message.c_str(), message.size(), 0))
		return 0;
	return 1;
}

int		Server::receive_data()
{
	std::list<User>::iterator	begin, end, tmp;

	begin = this->__users.begin();
	end = this->__users.end();
	while (begin != end)
	{
		tmp = begin;
		begin++;
		if (FD_ISSET((*tmp).get_fd(), &this->__sockets))
			this->read_socket(*tmp);
	}
	return 0;
}

int		Server::check_connection()
{
	std::list<User>::iterator	begin, end, tmp;
	std::vector<std::string>	quit;

	quit.push_back("QUIT!");
	quit.push_back(":User Has Been Detected as Unreachable");
	begin = this->__users.begin();
	end = this->__users.end();
	int		i;
	char	buffer[1];

	while (begin != end)
	{
		// std::cerr << "\t\treading ... " << (*begin).get_fd() << " ";
		i = recv((*begin).get_fd(), buffer, 1, MSG_PEEK);
		// std::cerr << "read: " << i << '\n';
		if (i == 0)
		{
			tmp = begin;
			begin++;
			std::cerr << "A bad read was detected\n";
			QUIT(*this, *tmp, quit);
		}
		else
			begin++;
	}
	// std::cerr << "number of users list " << this->__users.size() << std::endl;
	return 0;
}

User		*Server::getuserbynick(const std::string &nick)
{
	for (std::list<User>::iterator begin = this->__users.begin(); begin != this->__users.end(); begin++)
	{
		if ((*begin).get_nickname() == nick)
			return (&(*begin));
	}
	return nullptr;
}

User		*Server::getuserbyfd(int fd)
{
	for (std::list<User>::iterator begin = this->__users.begin(); begin != this->__users.end(); begin++)
	{
		if ((*begin).get_fd() == fd)
			return (&(*begin));
	}
	return nullptr;
}

int			Server::disconnect_user(const std::list<User>::iterator &it)
{
	this->__users.erase(it);
	std::cerr << "user disconnected !" << std::endl;
	return (0);
}

int			Server::disconnect_user(const User &u)
{
	std::list<User>::iterator	begin, end;
	// std::cerr << "nb_user: " << this->__users.size() << std::endl;

	begin = this->__users.begin();
	end = this->__users.end();
	while ((*begin).get_fd() != u.get_fd() && begin != end)
		begin++;
	if (begin == end)
	{
		std::cerr << "user not found !" << std::endl;
		return -1;
	}
	this->__users.erase(begin);
	std::cerr << "user disconnected !" << std::endl;
	return 0;
}

std::list<Channel>::iterator	Server::add_channel(std::string name, std::string password)
{
	this->__channels.push_back(Channel(name, password));
	// Channel chan(name, password);
	// this->__channels.insert(chan);
	return (this->__channels.end());
}

int 							Server::delete_channel(std::string name)
{
	for (std::list<Channel>::iterator begin = this->__channels.begin(); begin != this->__channels.end(); begin++)
	{
		if ((*begin).get_name() == name)
		{
			this->__channels.erase(begin);
			break ;
		}
	}
	return (this->__channels.size());
}

std::list<Channel>::iterator	Server::find_channel(char prefix, std::string name)
{
	std::list<Channel>::iterator	begin;

	for (begin = this->__channels.begin(); begin != this->__channels.end(); begin++)
	{
		if ((*begin).get_prefix() == prefix && (*begin).get_name() == name)
			break ;
	}
	return begin;
}
