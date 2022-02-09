#ifndef _MSG_PARSE_H__
#define _MSG_PARSE_H__

#include <iostream>
#include <vector>


class msg_parse
{
	private:
		std::string msg; // SHALL NOT exceed 512 characters in length
		//there are 510 characters maximum allowed for the command and its parameters
		std::string cmd; // The command MUST either be a valid IRC command or a three (3) digit number represented in ASCII text
		std::vector<char *> cmd_params;
		std::string space_par;

	public:
		msg_parse &operator=(const msg_parse & f);
		msg_parse(std::string buffer);
		msg_parse();
		int parser( void);
		int	command_checker(int *idx);
		void	params(int idx, char **tab);
		void	additional_param(char *tab);
		std::string get_cmd( void);
		std::vector<char *> get_cmd_params( void);
		std::string get_msg( void);
		std::string get_additional_param( void);
		~msg_parse();
};

msg_parse	message_splitter(char *&buffer, int ret);
void	check_command(msg_parse command);

#endif