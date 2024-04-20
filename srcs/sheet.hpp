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
	size_t getWidth() const {return width;}
	size_t getHeight() const {return height;} // unused
	Sheet& operator=(const Sheet&);
	ExprPointer* operator[](size_t i) {
		if (i < height)
			return table + i*width;
		throw "index out of range\n";
	}
	static int colNumber(std::string);
	static std::string colLetter (int);
	ExprPointer* parseCell(int col, int row) const;
	ExprPointer* parseCell(std::string col, int row) const;
	bool checkRow(int r) const {return r <= (int)height && r > 0;}
	bool checkCol(int col) const {return col <= (int)width && col > 0;}

	int getYCoord(ExprPointer* cell) const;
	int getXCoord(ExprPointer* cell) const;

	void copyTo(Sheet& sh) const;
	void resize(size_t w, size_t h, double fill = 0);

	void formattedPrint(std::ostream& os = std::cout) const;
	void printValues(std::ostream& os = std::cout) const;
	void printExpr(std::ostream& os = std::cout) const;

	~Sheet(){delete[] table;}
};


#endif
