#include "sheet.hpp"

#include <cctype>
#include <iomanip>

Sheet::Sheet(const Sheet& sh): width(sh.width), height(sh.height){
	table = new ExprPointer[sh.width * sh.height];
	for (size_t i = 0; i < width*height; i++) {
		table[i] = sh.table[i]->copy();
		table[i]->relocate(this);
	}
}

Sheet::Sheet(size_t w, size_t h, double fill) : width(w), height(h){
	table = new ExprPointer[width * height];
	for (size_t i = 0; i < width*height; i++) {
		table[i] = new NumberExpr(fill);
	}
}

Sheet& Sheet::operator=(const Sheet& sh){
	height = sh.height;
	width = sh.width;
	if (&sh != this){
		delete[] table;
		table = new ExprPointer[sh.width * sh.height];
		for (size_t i = 0; i < width*height; i++) {
			table[i] = sh.table[i]->copy();
			table[i]->relocate(this);
		}
	}
	return *this;
}

int Sheet::colNumber(std::string str) {
	int col = 0;
	for (char c : str) {
		if (!std::isalpha(c))
			throw "invalid column\n";
		col = col*26 + std::tolower(c) - 'a' + 1;
	}
	return col;
}

std::string Sheet::colLetter(int n) {
	std::string col = "";
	while (n > 0){
		col.insert(0, 1, (char)(n%26 + 'a' - 1));
		n = n/26;
	}
	return col;
}

ExprPointer* Sheet::parseCell(int col, int row) const {
	if (checkRow(row) && checkCol(col)) {
		return &(table[(row-1)*width + col -1]); //indexing from 0
	}
	throw "index out of range\n";
}
ExprPointer* Sheet::parseCell(std::string col, int row) const {
	int cn = colNumber(col);
	return parseCell(cn, row);
}

int Sheet::getYCoord(ExprPointer* cell) const {
	return (int)(cell - table) / (int)width;
}
int Sheet::getXCoord(ExprPointer* cell) const {
	return (int)(cell - getYCoord(cell)*(int)width - table);
}

void Sheet::copyTo(Sheet& sh) const {
	size_t minw = sh.width < width ? sh.width : width;
	size_t minh = sh.height < height ? sh.height : height;
	for (size_t row = 0; row < minh; row++){
		for (size_t col = 0; col < minw; col++){
			sh[row][col] = table[row*width + col]->copy();
			sh[row][col]->relocate(&sh);
		}
	}
}

void Sheet::resize(size_t w, size_t h, double fill){
	Sheet sh = Sheet(w, h, fill);
	copyTo(sh);
	*this = sh;
}

void Sheet::formattedPrint(std::ostream& os) const {
	if (height == 0 || width == 0) {
		os << "Sheet doesn't exists" << std::endl;
		return;
	}
	os << std::setw((int)std::log10(height)+2) << std::setfill(' ') << ' ';
	for (int col = 0; col < (int)width; col++) {
		os << colLetter(col+1) << "\t";
	}
	os << std::endl;
	for (int row = 0; row < (int)height; row++) {
		os << std::setw((int)std::log10(height)+1) << row+1 << "|";
		for (int col = 0; col < (int)width; col++) {
			try {
				os << table[row*width + col].evalMe() << "\t";
			} catch (const char* msg){
				os << "#ERR" << "\t";
			}
		}
		os << std::endl;
	}
}

void Sheet::printValues(std::ostream& os) const {
	for (int row = 0; row < (int)height; row++) {
		for (int col = 0; col < (int)width; col++) {
			try {
				os << table[row*width + col].evalMe() << ",";
			} catch (const char* msg){
				os << "#ERR" << ",";
			}
		}
		os << std::endl;
	}
}

void Sheet::printExpr(std::ostream& os) const {
	for (int row = 0; row < (int)height; row++) {
		for (int col = 0; col < (int)width; col++) {
			os << table[row*width + col]->show() << ",";
		}
		os << std::endl;
	}
}
