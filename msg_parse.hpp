#ifndef _MSG_PARSE_H__
#define _MSG_PARSE_H__

#include <iostream>
#include <vector>

class User;

class msg_parse
{
	private:
		std::string 		msg;
		std::string 		cmd;
		std::vector<char *> cmd_params;
		std::string 		space_par;
		int					pos;
		int					has_additional_param;

	public:
		msg_parse &operator=(const msg_parse & f);
		msg_parse(std::string buffer);
		msg_parse();
		~msg_parse();
		int 				parser( void);
		int					command_checker(int *idx);
		void				params(int idx, char **tab);
		void				additional_param(char *tab);
		std::string 		get_cmd( void);
		int					get_pos(void) const;
		int					get_has_additional_param(void) const;
		void				set_pos(int pos);
		void				set_has_additional_param(int pos);
		void 				set_msg( std::string buff);
		std::vector<char *> get_cmd_params( void);
		std::string 		get_msg( void);
		std::string 		get_additional_param( void);
};

msg_parse					message_splitter(const char *buffer, int ret, msg_parse &parsed_msg, User &user);
void						check_command(msg_parse command);

#endif