#include "msg_parse.hpp"
#include "User.hpp"


std::string	erase_CR_LF(std::string buff)
{
	size_t pos = std::string::npos;

    // while ((pos  = buff.find("\r\n") )!= std::string::npos)
    //     buff.erase(pos, 2);
    if ((pos  = buff.find("\r\n") )!= std::string::npos)
        buff.erase(pos, 2);
	return (buff);
}

msg_parse	message_splitter(const char *buffer, int ret, msg_parse &parsed_command, User &user)
{
	std::string buff = buffer;
	size_t pos = std::string::npos;

	if ((pos = buff.find("\r\n") ) == std::string::npos)
	{
		user.get_msgs().push_back(buff);
	}
	if (buff.find("\r\n") != std::string::npos)
	{
		user.get_msgs().push_back(buff);
		buff.clear();
		for (std::vector<std::string>::iterator it = user.get_msgs().begin(); it != user.get_msgs().end() ; it++)
		{
			buff = buff + *it;
		}
		buff = erase_CR_LF(buff);
		parsed_command.set_msg(buff);
		if (!parsed_command.parser())
			ret = 0;
		// print_command(parsed_command);
		user.get_msgs().clear();
	}
	return(parsed_command);
}

std::string msg_parse::get_msg(void)
{
	return (msg);
}

int msg_parse::get_pos(void) const
{
	return (this->pos);
}

void msg_parse::set_msg(std::string buff)
{
	msg = buff;
}

void msg_parse::set_pos(int pos)
{
	this->pos = pos;
}

msg_parse::msg_parse( void) : cmd() , cmd_params() , space_par() , msg()
{
}

msg_parse::msg_parse(std::string buffer) : cmd() , cmd_params() , space_par()
{
	msg = buffer;
}

int	msg_parse::command_checker(int *idx)
{
	int pos;
	int start_cmd = 0;
	while (msg[*idx] && (msg[*idx] == ' ' || msg[*idx] == '\t'))
	{
		start_cmd++;
		(*idx)++;
	}
	for (; msg[*idx] && msg[*idx] != ' '; (*idx)++)
	{
		if (!isupper(msg[*idx]))
		{
			// std::cerr << "Command missing" << std::endl;
			pos = msg.find(" ", start_cmd);
			cmd = msg.substr(start_cmd, pos - start_cmd);
			//an error MUST be sent back to the client and the parsing terminated
			return (0);
		}
	}
	if (*idx)
		cmd = msg.substr(start_cmd, *idx - start_cmd);
	// std::cout << "this is the command |" << cmd << "|" << std::endl;
	return (1);
}

std::string msg_parse::get_additional_param( void)
{
	return (space_par);
}

void msg_parse::params(int idx, char **tab)
{
	char *tmp_msg = (char *)msg.c_str() + idx;

	*tab = strtok(tmp_msg, " \t");
	// *tab = strtok(tmp_msg, "\t");
	while (*tab && *tab[0] != ':')
	{
		cmd_params.push_back(*tab);
		*tab = strtok(NULL, " \t");
	}
}

void	msg_parse::additional_param(char *tab)
{

	if (tab && tab[0] == ':')
	{
		while (tab)
		{
			space_par += tab;
			tab = strtok(NULL, " ");
			if (tab)
				space_par += " ";
		}
		space_par.erase(0, 1);
	}
}

int	msg_parse::parser( void)
{
	int index = 0;

	if (msg.empty())
		std::cout << "Empty msg" << std::endl; //should be silently ignored
	if (!command_checker(&index))
		return (0);
	char *tab;
	params(index, &tab);
	additional_param(tab);
	return (1);
}

std::string msg_parse::get_cmd( void)
{
	return (cmd);
}
std::vector<char *> msg_parse::get_cmd_params( void)
{
	return (cmd_params);
}

msg_parse &msg_parse::operator=(const msg_parse & f)
{
	this->msg = f.msg;
	this->cmd = f.cmd;
	this->cmd_params = f.cmd_params;
	this->space_par = f.space_par;
	return (*this);
}

msg_parse::~msg_parse()
{

}

// int main(int c, char **v)
// {
// 	if (c > 1)
// 	{
// 		std::string conc_msg;
// 		std::string space = " ";
// 		for (int i = 1; i < c; i++)
// 		{
// 			conc_msg += v[i] + space;
// 		}
// 		msg_parse msg(conc_msg);
// 		msg.parser();
// 		// std::cout << msg.get_msg() << std::endl;
// 		return (0);
// 	}
// 	std::cout << "Add some arguments bb" << std::endl;
// 	return (0);
// }