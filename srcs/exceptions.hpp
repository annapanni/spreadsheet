#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>

/**
*Saját exception osztály a szintaktikailag helytelen kifejezések értelmezésénél felmerülő hibákra.
*A szintakikai hiba részleteit a .what() tagfüggvényel kaphatjuk meg.
*/
class syntax_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};
/**
*Saját exception osztály a kifejezések kiértékelésekor felmerülő hibákra.
*A szintakikai hiba részleteit a .what() tagfüggvényel kaphatjuk meg.
*/
class eval_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};


#endif
