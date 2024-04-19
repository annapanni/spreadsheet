#include "console.hpp"

void Console::help(){
	os << "Available commands: \n\
	\t new [int w] [int h] - create a new sheet \n\
	\t resize [int w] [int h] - resize current table \n\
	\t print - print sheet \n\
	\t set [cell] [expression] - set a given cell in sheet \n\
	\t pull [cell] [cell] - relative copy of the expression of the first cell until the last \n\
	\t show [cell] - display contents of given cell \n\
	\t help - display available commands \n\
	\t exit - close program\n";
}

void Console::createNew() {
	size_t w, h;
	is >> w >> h;
	sh = Sheet(w, h);
}

void Console::resize() {
	size_t w, h;
	is >> w >> h;
	sh.resize(w, h);
}

void Console::set() {
	std::string cellstr;
	is >> cellstr;
	try	{
		CellId cid(cellstr);
		if (sh.checkRow(cid.row) && sh.checkCol(cid.colNum)){
			std::string inp;
			is >> inp;
			Parser(inp).parseTo(&sh, sh[cid.row-1][cid.colNum-1]);
		} else {
			os << "index out of range\n";
		}
	} catch (const char* msg){os << msg;}
}

void Console::pull() {
	std::string cellstr1, cellstr2;
	is >> cellstr1 >> cellstr2;
	try {
		CellRefExpr start = CellRefExpr(cellstr1, &sh);
		int sx = sh.getXCoord(start.getPtr());
		int sy = sh.getYCoord(start.getPtr());
		Range range(new CellRefExpr(cellstr1, &sh), new CellRefExpr(cellstr2, &sh));
		for (Range::iterator cell = range.begin(); cell != range.end(); cell++) {
			*cell = (*start.getPtr())->copy();
			(*cell)->shift(sh.getXCoord(&*cell)-sx, sh.getYCoord(&*cell)-sy);
		}
	} catch (const char* msg) {os << msg;}
}

void Console::show() {
	std::string cellstr;
	is >> cellstr;
	try	{
		CellId cid(cellstr);
		if (sh.checkRow(cid.row) && sh.checkCol(cid.colNum)){
			os << sh[cid.row-1][cid.colNum-1]->show() << " = ";
			os << sh[cid.row-1][cid.colNum-1].evalMe() << '\n';
		} else {
			os << "index out of range\n";
		}
	} catch (const char* msg) {os << msg;}
}

void Console::readCommand(){
	std::string command;
	is >> command;
	if (command == "print") {
		print();
	} else if (command == "set") {
		set();
	} else if (command == "show") {
		show();
	}  else if (command == "pull") {
		pull();
	} else if (command == "new") {
		createNew();
	} else if (command == "resize") {
		resize();
	} else if (command == "help") {
		help();
	} else if (command == "exit") {
		exit();
	} else {
		os << "\ninvalid command\n";
	}
}
