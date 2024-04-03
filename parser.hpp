#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>

#include "token.hpp"
#include "expression.hpp"
#include "sheet.hpp"


class Parser {
	std::vector<Token*> tokens;
	size_t current = 0;

	bool atEnd() const {return current >= tokens.size();}
	Token* prev() const {return tokens[current-1];}
	bool check(Token_type tt) const {return !atEnd() && tokens[current]->getType()==tt;}
	bool match(Token_type tt);
	Token* consume(Token_type tt, const char* msg);

	Expression* expression(Sheet* shp = NULL);
	Expression* factor(Sheet* shp = NULL);
	Expression* unary(Sheet* shp = NULL);
	Expression* function(Sheet* shp = NULL);
	Expression* primary(Sheet* shp = NULL);
	CellRefExpr* cell(Sheet* shp = NULL);
public:
	Parser(std::string input);

	void addToken(Token_type t);
	void addToken(std::string s);
	void addToken(double n);
	void addTokenFromStr(std::string& str_buffer);

	Expression* parse(Sheet* shp = NULL);

	void show(){
		for (Token*& t : tokens) {
			t->show();
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

	~Parser() {
		for (size_t i = 0; i < tokens.size(); i++) {
			delete tokens[i];
		}
	}
};

Operand* operandFromToken(Token_type tt, Expression* lhs, Expression* rhs);

#endif
