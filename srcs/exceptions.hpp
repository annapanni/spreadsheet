#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


#include <exception>

class syntax_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};
class eval_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};


#endif
