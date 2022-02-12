#ifndef __UMODES_HPP___
#define __UMODES_HPP___

#include <string>

class UModes
{
private:
	bool	__a; // away
	bool	__i; // invisible
	bool	__w; // walops
	bool	__r; // restricted
	bool	__o; // operator
	bool	__O; // local operator
	bool	__s; // server notcies

public:
	// * Copliens form
	UModes();
	UModes(const UModes &rhs);
	~UModes();
	UModes		&operator=(const UModes &rhs);

	bool		get_a() const;
	bool		get_i() const;
	bool		get_w() const;
	bool		get_r() const;
	bool		get_o() const;
	bool		get_O() const;
	bool		get_s() const;

	void		set_a(bool p);
	void		set_i(bool p);
	void		set_w(bool p);
	void		set_r(bool p);
	void		set_o(bool p);
	void		set_O(bool p);
	void		set_s(bool p);
	void		set(char a);
	void		unset(char a);

	UModes		operator-(const char c);
	UModes		operator-=(const char c);
	UModes		operator+(const char c);
	UModes		operator+=(const char c);

	UModes		update(const std::string &str);
};

#endif