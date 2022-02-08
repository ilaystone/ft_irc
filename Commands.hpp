#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include <vector>
#include <string>
#include "Server.hpp"

// int call_command(Server &serv, User &u, std::vector<std::string> message);

// int AWAY(Server &serv, User &u, std::vector<std::string> message);
// int JOIN(Server &serv, User &u, std::vector<std::string> message);
// int LUSERS(Server &serv, User &u, std::vector<std::string> message);
// int MOTD(Server &serv, User &u, std::vector<std::string> message); // * Configuration file read
// int NOTICE(Server &serv, User &u, std::vector<std::string> message);
// int NICK(Server &serv, User &u, std::vector<std::string> message);
// int PART(Server &serv, User &u, std::vector<std::string> message);
// int PING(Server &serv, User &u, std::vector<std::string> message);
// int PONG(Server &serv, User &u, std::vector<std::string> message);
// int PRIVMSG(Server &serv, User &u, std::vector<std::string> message);
int QUIT(Server &serv, User &u, std::vector<std::string> message);
// int USER(Server &serv, User &u, std::vector<std::string> message);
// int WHOIS(Server &serv, User &u, std::vector<std::string> message);
// int OPER(Server &serv, User &u, std::vector<std::string> message);
// int PASS(Server &serv, User &u, std::vector<std::string> message);
// int ISON(Server &serv, User &u, std::vector<std::string> message);


#endif