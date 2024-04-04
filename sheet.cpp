#include "sheet.hpp"

#include <cctype>
#include <iomanip>

Sheet::Sheet(const Sheet& sh): width(sh.width), height(sh.height){
	table = new ExprPointer[sh.width * sh.height];
	for (size_t i = 0; i < width*height; i++) {
		table[i] = sh.table[i]->copy();
	}
}

Sheet::Sheet(size_t w, size_t h) : width(w), height(h){
	table = new ExprPointer[width * height];
	for (size_t i = 0; i < width*height; i++) {
		table[i] = new NumberExpr(0);
	}
}

Sheet::Sheet(std::vector<double> vec, size_t w) : width(w), height(vec.size()/w){
	table = new ExprPointer[vec.size()];
	for (size_t i = 0; i < vec.size(); i++) {
		table[i] = ExprPointer(new NumberExpr(vec[i]));
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
		}
	}
	return *this;
}

size_t Sheet::colNumber(std::string str) const {
	size_t col = 0;
	for (char c : str) {
		if (!std::isalpha(c))
			throw "invalid column\n";
		col = col*26 + std::tolower(c) - 'a' + 1;
	}
	return col;
}

std::string Sheet::colLetter(size_t n) const {
	std::string col = "";
	while (n > 0){
		col.insert(0, 1, (char)n%26 + 'a' - 1);
		n = n/26;
	}
	return col;
}

void Sheet::print() const {
	if (height == 0 || width == 0) {
		std::cout << "Sheet doesn't exists" << std::endl;
		return;
	}
	std::cout << std::setw((int)std::log10(height)+2) << std::setfill(' ') << ' ';
	for (size_t col = 0; col < width; col++) {
		std::cout << colLetter(col+1) << "\t";
	}
	std::cout << std::endl;
	for (size_t row = 0; row < height; row++) {
		std::cout << std::setw((int)std::log10(height)+1) << row+1 << "|";
		for (size_t col = 0; col < width; col++) {
			try {
				std::cout << table[row*width + col]->eval() << "\t";
			} catch (const char* msg){
				std::cout << "#ERR" << "\t";
			}
		}
		std::cout << std::endl;
	}
}
