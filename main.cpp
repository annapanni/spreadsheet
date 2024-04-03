#include "parser.hpp"
#include "sheet.hpp"

int main(void) {
	Parser p("65 + c3 - sum(s245 : d45)*12 + 12/23");
	p.show();
	Expression* e = p.parse();
	if (e != NULL){
		std::cout << e->show();
	} else {
		std::cout << "\nSomething went wrong";
	}
	std::cout << std::endl;
	delete e;


	Sheet sh(std::vector<double>{1,2,3,4}, 2);
	std::cout << sh.getWidth() << " " << sh.getHeight() << std::endl;

	Parser p2("c1");
	Expression* expr = p2.parse(&sh);
	if (expr != NULL){
		std::cout << expr->show() << std::endl;
		try{
			std::cout << expr->eval() << std::endl;
		} catch (const char* msg){
			std::cout << msg << std::endl;
		}
	} else {
		std::cout << "\nSomething went wrong with parsing\n";
	}

	delete expr;


	return 0;
}
