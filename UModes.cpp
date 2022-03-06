#include "UModes.hpp"
#include <iostream>

UModes::UModes()
:	__a(0),
	__i(0),
	__w(0),
	__r(0),
	__o(0),
	__O(0),
	__s(0)
{
	return ;
}

UModes::UModes(const UModes &rhs)
:	__a(rhs.__a),
	__i(rhs.__i),
	__w(rhs.__w),
	__r(rhs.__r),
	__o(rhs.__o),
	__O(rhs.__O),
	__s(rhs.__s)
{
	return ;
}

UModes::~UModes()
{
	return ;
}

UModes		&UModes::operator=(const UModes &rhs)
{
	this->__a = rhs.__a;
	this->__i = rhs.__i;
	this->__w = rhs.__w;
	this->__r = rhs.__r;
	this->__o = rhs.__o;
	this->__O = rhs.__O;
	this->__s = rhs.__s;
	return *this;
}

bool		UModes::get_a() const
{
	return this->__a;
}

bool		UModes::get_i() const
{
	return this->__i;
}

bool		UModes::get_w() const
{
	return this->__w;
}

bool		UModes::get_r() const
{
	return this->__r;
}

bool		UModes::get_o() const
{
	return this->__o;
}

bool		UModes::get_O() const
{
	return this->__O;
}

bool		UModes::get_s() const
{
	return this->__s;
}

void		UModes::set_a(bool p = true)
{
	this->__a = p;
}

void		UModes::set_i(bool p = true)
{
	this->__i = p;
}

void		UModes::set_w(bool p = true)
{
	this->__w = p;
}

void		UModes::set_r(bool p = true)
{
	this->__r = p;
}

void		UModes::set_o(bool p = true)
{
	this->__o = p;
}

void		UModes::set_O(bool p = true)
{
	this->__O = p;
}

void		UModes::set_s(bool p = true)
{
	this->__r = p;
}

void		UModes::set(char m)
{
	switch (m)
	{
		case '1':
		case 'a':
			set_a();
			break;
		case '2':
		case 'i':
				set_i();
			break;
		case '3':
		case 'w':
			set_w();
			break;
		case '4':
		case 'r':
			set_r();
			break;
		case '5':
		case 'o':
			set_o();
			break;
		case '6':
		case 'O':
			set_O();
			break;
		case '7':
		case 's':
			set_s();
			break;
		
		default:
			break;
	}
}
void		UModes::unset(char m)
{
	if (m == 'a')
		set_a(false);
	else if (m == 'i')
		set_i(false);
	else if (m == 'w')
		set_w(false);
	else if (m == 'r')
		set_r(false);
	else if (m == 'o')
		set_o(false);
	else if (m == 'O')
		set_O(false);
	else if (m == 's')
		set_s(false);
}

UModes		UModes::operator-(const char c)
{
	UModes	ret(*this);

	ret -= c;
	return (ret);
}

UModes		UModes::operator-=(const char c)
{
	if (c == 'i')
		this->__i = false;
	else if (c == 'w')
		this->__w = false;
	else if (c == 'o')
		this->__o = false;
	else if (c == 'O')
		this->__O = false;
	else if (c == 's')
		this->__s = false;
	return *this;
}

UModes		UModes::operator+(const char c)
{
	UModes	ret(*this);

	ret -= c;
	return (ret);
}

UModes		UModes::operator+=(const char c)
{
	if (c == 'i')
		this->__i = true;
	else if (c == 'w')
		this->__w = true;
	else if (c == 'r')
		this->__o = true;
	else if (c == 's')
		this->__s = true;
	return *this;
}


UModes		UModes::update(const std::string &str)
{
	int		i = 0;

	if (str[0] == '+')
	{
		while (str[i])
			*this += str[i++];
	}
	else if (str[0] == '-')
	{
		while (str[i])
			*this -= str[i++];
	}
	return *this;
}
