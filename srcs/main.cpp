#include <iostream>
#include "console.hpp"

int main(void) {
	Console con(std::cout, std::cin);
	con.help();
	while(!con.isClosed()){
		std::cout << "> ";
		con.readCommand();
	}
	return 0;
}
