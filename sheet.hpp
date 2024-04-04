#ifndef SHEET_HPP
#define SHEET_HPP

#include <string>
#include <vector>
#include <math.h>

#include "expression.hpp"

class Sheet {
	ExprPointer* table;
	size_t width;
	size_t height;
public:
	Sheet(const Sheet&);
	Sheet(ExprPointer* t=NULL, size_t w=0, size_t h=0) : table(t), width(w), height(h) {}
	Sheet(size_t w, size_t h);
	Sheet(std::vector<double> vec, size_t w);
	size_t getWidth() const {return width;}
	size_t getHeight() const {return height;}
	Sheet& operator=(const Sheet&);
	/*
	const Expression** operator[](size_t i)const {
		if (i < height)
			return table + i*width;
		throw "index out of range\n";
	}*/
	ExprPointer* operator[](size_t i) {
		if (i < height)
			return table + i*width;
		throw "index out of range\n";
	}
	size_t colNumber(std::string) const;
	std::string colLetter (size_t) const;
	bool checkRow(size_t r) const {return r <= height && r > 0;}
	bool checkCol(size_t col) const {return col <= width && col > 0;}

	void print() const;

	~Sheet(){
		delete[] table;
	}
};


#endif
