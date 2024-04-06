#include <iostream>
#include "parser.hpp"
#include "sheet.hpp"
#include <string>

void help(){
	std::cout << "Available commands: \n\
	\t print - print sheet \n\
	\t set (int row) (int col) - set a given cell in sheet (indexing from 0) \n\
	\t show (int row) (int col) - display contents of given cell (indexing from 0) \n\
	\t new (int w) (int h) - create a new sheet \n\
	\t resize (int w) (int h) - resize current table \n\
	\t help - display available commands \n\
	\t exit - close program\n";
}

int main(void) {
	bool exit = false;
	Sheet sh;
	std::string command;
	help();
	while(!exit){
		std::cout << "> ";
		std::cin >> command;
		if (command == "print") {
			sh.print();
		} else if (command == "set") {
			std::string cellstr;
			std::cin >> cellstr;
			CellId cid(cellstr);
			if (sh.checkRow(cid.row) && sh.checkCol(cid.colNum)){
				std::string inp;
				std::cin >> inp;
				Parser(inp).parseTo(&sh, sh[cid.row-1][cid.colNum-1]);
			} else {
				std::cout << "\nindex out of range\n";
			}
		} else if (command == "show") {
			std::string cellstr;
			std::cin >> cellstr;
			CellId cid(cellstr);
			if (sh.checkRow(cid.row) && sh.checkCol(cid.colNum)){
				std::cout << sh[cid.row-1][cid.colNum-1]->show() << " = ";
				try	{
					std::cout << sh[cid.row-1][cid.colNum-1].evalMe() << '\n';
				} catch (const char* msg) {
					std::cout << msg;
				}
			} else {
				std::cout << "\nindex out of range\n";
			}
		} else if (command == "new") {
			size_t w, h;
			std::cin >> w >> h;
			sh = Sheet(w, h);
		} else if (command == "resize") {
			size_t w, h;
			std::cin >> w >> h;
			sh.resize(w, h);
		} else if (command == "help") {
			help();
		} else if (command == "exit") {
			exit = true;
		} else {
			std::cout << "\ninvalid command\n";
		}
	}
	return 0;
}
