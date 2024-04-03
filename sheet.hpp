#ifndef SHEET_HPP
#define SHEET_HPP

#include <string>
#include <vector>

#include "expression.hpp"

class Sheet {
	ExprPointer* table;
	size_t width;
	size_t height;
public:
	Sheet(const Sheet&);
	Sheet(ExprPointer* t=NULL, size_t w=0, size_t h=0) : table(t), width(w), height(h) {}
	Sheet(std::vector<double> vec, size_t w);
	size_t getWidth() const {return width;}
	size_t getHeight() const {return height;}
	/*
	const Expression** operator[](size_t i)const {
		if (i < height)
			return table + i*width;
		throw "index out of range";
	}*/
	ExprPointer* operator[](size_t i) {
		if (i < height)
			return table + i*width;
		throw "index out of range";
	}
	size_t colNumber(std::string);
	std::string colLetter(int);
	bool checkRow(size_t r){return r <= height && r > 0;}
	bool checkCol(size_t col){return col <= width && col > 0;}

	~Sheet(){
		for (size_t i = 0; i < width*height; i++) {
			delete table[i];
		}
		delete[] table;
	}
};


#endif
