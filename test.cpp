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

	sh[0][0] = new NumberExpr(5);

	Parser p2("sum(b2:b2)");
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


	sh.print();
	sh[0][1] = Parser("b2").parse(&sh);

	sh[0][1] = Parser("sum(b2:b2)").parse(&sh);
	sh[1][0] = Parser("b1").parse(&sh);

	sh.print();

	Sheet sh2(std::vector<double>{1,2,3,4}, 4);
	sh2.print();

	delete expr;
	return 0;
}
