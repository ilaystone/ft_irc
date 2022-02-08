#include "UModes.hpp"

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

void		UModes::set_a(bool p)
{
	this->__a = p;
}

void		UModes::set_o(bool p)
{
	this->__o = p;
}

void		UModes::set_O(bool p)
{
	this->__O = p;
}

void		UModes::set_r(bool p)
{
	this->__r = p;
}

void		UModes::set(int i)
{
	i /= 4;
	if (i % 2 == 1)
	{
		*this += 'w';
		i--;
	}
	i /= 2;
	if (i % i == 1)
	{
		*this += 'i';
		i--;
	}
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
