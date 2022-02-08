#include "CModes.hpp"

CModes::CModes()
:	__O(false),
	__o(false),
	__v(false),
	__a(false),
	__i(false),
	__m(false),
	__n(false),
	__q(false),
	__p(false),
	__s(false),
	__r(false),
	__t(false),
	__k(false),
	__l(false),
	__b(false),
	__e(false),
	__I(false)
{
	return ;
}

CModes::CModes(const CModes &rhs)
:	__O(rhs.__O),
	__o(rhs.__o),
	__v(rhs.__v),
	__a(rhs.__a),
	__i(rhs.__i),
	__m(rhs.__m),
	__n(rhs.__n),
	__q(rhs.__q),
	__p(rhs.__p),
	__s(rhs.__s),
	__r(rhs.__r),
	__t(rhs.__t),
	__k(rhs.__k),
	__l(rhs.__l),
	__b(rhs.__b),
	__e(rhs.__e),
	__I(rhs.__I)
{
	return ;
}

CModes::~CModes()
{
	return ;
}

CModes	&CModes::operator=(const CModes &rhs)
{
	this->__O = rhs.__O;
	this->__o = rhs.__o;
	this->__v = rhs.__v;
	this->__a = rhs.__a;
	this->__i = rhs.__i;
	this->__m = rhs.__m;
	this->__n = rhs.__n;
	this->__q = rhs.__q;
	this->__p = rhs.__p;
	this->__s = rhs.__s;
	this->__r = rhs.__r;
	this->__t = rhs.__t;
	this->__k = rhs.__k;
	this->__l = rhs.__l;
	this->__b = rhs.__b;
	this->__e = rhs.__e;
	this->__I = rhs.__I;

	return *this;
}

bool	CModes::get_O() const { return this->__O; }
bool	CModes::get_o() const { return this->__o; }
bool	CModes::get_v() const { return this->__v; }
bool	CModes::get_a() const { return this->__a; }
bool	CModes::get_i() const { return this->__i; }
bool	CModes::get_m() const { return this->__m; }
bool	CModes::get_n() const { return this->__n; }
bool	CModes::get_q() const { return this->__q; }
bool	CModes::get_p() const { return this->__p; }
bool	CModes::get_s() const { return this->__s; }
bool	CModes::get_r() const { return this->__r; }
bool	CModes::get_t() const { return this->__t; }
bool	CModes::get_k() const { return this->__k; }
bool	CModes::get_l() const { return this->__l; }
bool	CModes::get_b() const { return this->__b; }
bool	CModes::get_e() const { return this->__e; }
bool	CModes::get_I() const { return this->__I; }

void	CModes::set_O(bool val) { this->__O = val; }
void	CModes::set_o(bool val) { this->__o = val; }
void	CModes::set_v(bool val) { this->__v = val; }
void	CModes::set_a(bool val) { this->__a = val; }
void	CModes::set_i(bool val) { this->__i = val; }
void	CModes::set_m(bool val) { this->__m = val; }
void	CModes::set_n(bool val) { this->__n = val; }
void	CModes::set_q(bool val) { this->__q = val; }
void	CModes::set_p(bool val) { this->__p = val; }
void	CModes::set_s(bool val) { this->__s = val; }
void	CModes::set_r(bool val) { this->__r = val; }
void	CModes::set_t(bool val) { this->__t = val; }
void	CModes::set_k(bool val) { this->__k = val; }
void	CModes::set_l(bool val) { this->__l = val; }
void	CModes::set_b(bool val) { this->__b = val; }
void	CModes::set_e(bool val) { this->__e = val; }
void	CModes::set_I(bool val) { this->__I = val; }
