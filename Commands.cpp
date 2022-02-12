#include "Commands.hpp"

// int		call_command(Server &serv, User &user, std::vector<std::string> message)
// {
// 	if (message.size() == 0)
// 		return -1;
// 	std::cerr << "Command received: " << message[0] << std::endl;

// 	if (message[0] == "NICK")
// 		return (NICK(serv, user, message));
// 	else if (message[0] == "QUIT")
// 		return (QUIT(serv, user, message));
// 	else if (message[0] == "USER")
// 		return (USER(serv, user, message));
// 	else if (message[0] == "PONG")
// 	{
// 		std::cerr << "PONG: silencing command!" << std::endl;
// 		return (0);
// 	}
// 	else if (message[0] == "PING")
// 		return (PING(serv, user, message));
// 	else if (user.get_nickname() == "" || user.get_username() == "")
// 	{
// 		send_reply(451, serv, user, message);
// 		return 1;
// 	}
// 	std::cerr << "Command not found" << std::endl;
// 	return 404;
// }

// int		NICK(Server &serv, User &user, std::vector<std::string> message)
// {
// 	if (message.size() < 2)
// 		std::cerr << "nick command error" << std::endl;
// 	if (serv.getuserbynick(message[1]))
// 		std::cerr << "Nick used" << std::endl;
// 	else
// 		user.set_nickname(message[1]);
// 	return 0;
// }

int QUIT(Server &serv, User &u, std::vector<std::string> message)
{	
	serv.disconnect_user(u);
	return (0);
}
