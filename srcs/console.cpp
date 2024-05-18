#include "console.hpp"

void Console::help(){
	ostream << "Available commands: \n\
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
	istream >> w >> h;
	sh = Sheet(w, h);
}

void Console::resize() {
	size_t w, h;
	istream >> w >> h;
	sh.resize(w, h);
}

void Console::exportValues() {
	std::ofstream ofile;
	try	{
		std::string fname;
		istream >> fname;
		ofile.open(fname + ".csv");
		sh.printValues(ofile);
	} catch (...){
		ostream << "Export failed\n";
	}
	ofile.close();
}

void Console::save() {
	std::ofstream ofile;
	try	{
		std::string fname;
		istream >> fname;
		ofile.open(fname + ".csv");
		sh.printExpr(ofile);
	} catch (...){
		ostream << "Export failed\n";
	}
	ofile.close();
}

void Console::load() {
	std::ifstream ifile;
	std::string fname;
	unsigned int w = 0, h = 0;
	istream >> fname;
	try	{ifile.open(fname + ".csv");}
	catch (...) {ostream << "Load failed\n"; return;}
	//counting lines
	std::string line, word;
	if (getline(ifile, line)) h++;
	std::stringstream linestream(line);
	while (getline(linestream, word, ',')) {w++;}
	while (getline(ifile, line)) {h++;}
	ifile.close();
	//opening file again
	try	{ifile.open(fname + ".csv");}
	catch (...) {ostream << "Load failed\n"; return;}
	Sheet newsh(w, h, 0);
	unsigned int row = 0;
	while (getline(ifile, line) && row < h) {
		unsigned int col = 0;
		std::stringstream linestream(line);
		while (getline(linestream, word, ',') && col < w) {
			try {Parser(word).parseTo(&newsh, newsh[row][col]);}
			catch (const syntax_error& err) {}
			catch (const eval_error& err) {}
			col++;
		}
		row++;
	}
	ifile.close();
	sh = newsh;
}

void Console::set() {
	std::string cellstr;
	istream >> cellstr;
	try	{
		CellId cid(cellstr);
		if (sh.checkRow(cid.getRow()) && sh.checkCol(cid.getColNum())){
			std::string inp;
			istream >> inp;
			Parser(inp).parseTo(&sh, sh[cid.getRow()-1][cid.getColNum()-1]);
		} else {
			ostream << "index out of range\n";
		}
	} catch (const syntax_error& err) {ostream << "syntax error: " << err.what() << std::endl;
	} catch (const eval_error& err) {ostream << "evaluation error: " << err.what() << std::endl;}
}

void Console::pull() {
	std::string cellstr1, cellstr2;
	istream >> cellstr1 >> cellstr2;
	try {
		CellRefExpr start = CellRefExpr(cellstr1, &sh);
		unsigned int startx = sh.getXCoord(start.getPtr());
		unsigned int starty = sh.getYCoord(start.getPtr());
		Range range(new CellRefExpr(cellstr1, &sh), new CellRefExpr(cellstr2, &sh));
		for (Range::iterator cell = range.begin(); cell != range.end(); cell++) {
			*cell = (*start.getPtr())->copy();
			(*cell)->shift(sh.getXCoord(&*cell)-startx, sh.getYCoord(&*cell)-starty);
		}
	} catch (const syntax_error& err) {ostream << "syntax error: " << err.what() << std::endl;
	} catch (const eval_error& err) {ostream << "evaluation error: " << err.what() << std::endl;}
}

void Console::show() {
	std::string cellstr;
	istream >> cellstr;
	try	{
		CellId cid(cellstr);
		if (sh.checkRow(cid.getRow()) && sh.checkCol(cid.getColNum())){
			ostream << sh[cid.getRow()-1][cid.getColNum()-1]->show() << " = ";
			ostream << sh[cid.getRow()-1][cid.getColNum()-1].evalMe() << '\n';
		} else {
			ostream << "index out of range\n";
		}
	} catch (const syntax_error& err) {ostream << "syntax error: " << err.what() << std::endl;
	} catch (const eval_error& err) {ostream << "evaluation error: " << err.what() << std::endl;}
}

void Console::readCommand(){
	std::string command;
	istream >> command;
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
		ostream << "invalid command\n";
	}
}
