#ifndef __CMODES_HPP__
#define __CMODES_HPP__

#include <string>

class CModes
{
private:
	bool	__O; // - give "channel creator" status;
	bool	__o; // - give/take channel operator privilege;
	bool	__v; // - give/take the voice privilege;

	bool	__a; // - toggle the anonymous channel flag;
	bool	__i; // - toggle the invite-only channel flag;
	bool	__m; // - toggle the moderated channel;
	bool	__n; // - toggle the no messages to channel from clients on the outside;
	bool	__q; // - toggle the quiet channel flag;
	bool	__p; // - toggle the private channel flag;
	bool	__s; // - toggle the secret channel flag;
	bool	__r; // - toggle the server reop channel flag;
	bool	__t; // - toggle the topic settable by channel operator only flag;

	bool	__k; // - set/remove the channel key (password);
	bool	__l; // - set/remove the user limit to channel;

	bool	__b; // - set/remove ban mask to keep users out;
	bool	__e; // - set/remove an exception mask to override a ban mask;
	bool	__I; // - set/remove an invitation mask to automatically override the invite-only flag;

public:
	// * Copliens form
	CModes();
	CModes(const CModes &rhs);
	~CModes();
	CModes	&operator=(const CModes &rhs);

	// * Getter
	bool	get_O() const;
	bool	get_o() const;
	bool	get_v() const;
	bool	get_a() const;
	bool	get_i() const;
	bool	get_m() const;
	bool	get_n() const;
	bool	get_q() const;
	bool	get_p() const;
	bool	get_s() const;
	bool	get_r() const;
	bool	get_t() const;
	bool	get_k() const;
	bool	get_l() const;
	bool	get_b() const;
	bool	get_e() const;
	bool	get_I() const;

	// * Setter
	void	set_O(bool val);
	void	set_o(bool val);
	void	set_v(bool val);
	void	set_a(bool val);
	void	set_i(bool val);
	void	set_m(bool val);
	void	set_n(bool val);
	void	set_q(bool val);
	void	set_p(bool val);
	void	set_s(bool val);
	void	set_r(bool val);
	void	set_t(bool val);
	void	set_k(bool val);
	void	set_l(bool val);
	void	set_b(bool val);
	void	set_e(bool val);
	void	set_I(bool val);
};

#endif