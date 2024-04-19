#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include "parser.hpp"
#include "sheet.hpp"
#include <string>

class Console {
	Sheet sh;
	std::ostream& os;
	std::istream& is;
	bool closed = false;
public:
	Console() : sh(Sheet()), os(std::cout), is(std::cin) {}
	Console(Sheet sh, std::ostream& os, std::istream& is) : sh(sh), os(os), is(is) {}
	Console(std::ostream& os, std::istream& is) : sh(Sheet()), os(os), is(is) {}

	bool isClosed() const {return closed;}

	void help();

	void createNew();
	void resize();
	void print() {sh.print(os);}
	void set();
	void pull();
	void show();
	void exit() {closed = true;}

	void readCommand();
};


#endif
