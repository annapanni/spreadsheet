#include "console.hpp"

void Console::help(){
	os << "Available commands: \n\
	\t new [int w] [int h] - create a new sheet \n\
	\t resize [int w] [int h] - resize current table \n\
	\t print - print sheet \n\
	\t set [cell] [expression] - set a given cell in sheet \n\
	\t pull [cell] [cell] - relative copy of the expression of the first cell until the last \n\
	\t show [cell] - display contents of given cell \n\
	\t export [filename] - exports the values of the sheet in csv format (extension added automatically) \n\
	\t save [filename] - saves the expressions in the sheet in csv format (extension added automatically) \n\
	\t load [filename] - loads sheet from csv file (extension added automatically) \n\
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

void Console::exportValues() {
	std::ofstream of;
	try	{
		std::string fname;
		is >> fname;
		of.open(fname + ".csv");
		sh.printValues(of);
	} catch (...){
		os << "Export failed\n";
	}
	of.close();
}
void Console::save() {
	std::ofstream of;
	try	{
		std::string fname;
		is >> fname;
		of.open(fname + ".csv");
		sh.printExpr(of);
	} catch (...){
		os << "Export failed\n";
	}
	of.close();
}

void Console::load() {
	std::ifstream ifile;
	std::string fname;
	int w = 0, h = 0;
	is >> fname;
	try	{ifile.open(fname + ".csv");}
	catch (...) {os << "Load failed\n"; return;}
	//counting lines
	std::string line, word;

	if (getline(ifile, line)) h++;
	std::stringstream linestream(line);
	while (getline(linestream, word, ',')) {w++;}
	while (getline(ifile, line)) {h++;}
	ifile.close();
	//opening file again
	try	{ifile.open(fname + ".csv");}
	catch (...) {os << "Load failed\n"; return;}
	Sheet newsh(w, h, 0);
	int row = 0, col = 0;
	while (getline(ifile, line)) {
		std::stringstream linestream(line);
		while (getline(linestream, word, ',')) {
			if (col >= w) {os << "incorrent file format\n"; return;}
			try {Parser(word).parseTo(&newsh, newsh[row][col]);}
			catch (const char*) {}
			col++;
		}
		row++;
		col = 0;
	}
	ifile.close();
	newsh.formattedPrint();
	sh = newsh;
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
	} else if (command == "load") {
		load();
	} else if (command == "save") {
		save();
	} else if (command == "export") {
		exportValues();
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
