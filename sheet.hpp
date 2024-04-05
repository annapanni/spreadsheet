#ifndef SHEET_HPP
#define SHEET_HPP

#include <string>
#include <vector>
#include <math.h>

#include "expression_core.hpp"

class Sheet {
	ExprPointer* table;
	size_t width;
	size_t height;
public:
	Sheet() : table(NULL), width(0), height(0) {}
	Sheet(const Sheet&);
	Sheet(size_t w, size_t h, double fill = 0);
	Sheet(std::vector<double> vec, size_t w);
	size_t getWidth() const {return width;}
	size_t getHeight() const {return height;} // unused
	Sheet& operator=(const Sheet&);
	ExprPointer* operator[](size_t i) {
		if (i < height)
			return table + i*width;
		throw "index out of range\n";
	}
	size_t colNumber(std::string) const;
	std::string colLetter (size_t) const;
	ExprPointer* parseCell(std::string col, size_t row) const;
	bool checkRow(size_t r) const {return r <= height && r > 0;}
	bool checkCol(size_t col) const {return col <= width && col > 0;}

	void copyTo(Sheet& sh) const;
	void resize(size_t w, size_t h, double fill = 0);

	void print() const;

	~Sheet(){delete[] table;}
};


#endif
