#include "sheet.hpp"

#include <cctype>

Sheet::Sheet(const Sheet& sh): width(sh.width), height(sh.height){
	table = new Expression*[sh.width * sh.height];
	for (size_t i = 0; i < width*height; i++) {
		table[i] = sh.table[i]->copy();
	}
}

Sheet::Sheet(std::vector<double> vec, size_t w) : width(w), height(vec.size()/w){
	table = new Expression*[vec.size()];
	for (size_t i = 0; i < vec.size(); i++) {
		table[i] = ExprPointer(new NumberExpr(vec[i]));
	}
}

size_t Sheet::colNumber(std::string str){
	size_t col = 0;
	for (char c : str) {
		if (!std::isalpha(c))
			throw "invalid column";
		col = col*26 + std::tolower(c) - 'a' + 1;
	}
	return col;
}

std::string Sheet::colLetter(int n){
	std::string col = "";
	while (n > 0){
		col.insert(0, 1, (char)n%26 + 'a' - 1);
		n = n/26;
	}
	return col;
}
