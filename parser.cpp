#include <cctype>
#include "parser.hpp"

void Parser::addToken(Token_type t){
	tokens.push_back(new Token(t));
}
void Parser::addToken(std::string s){
	tokens.push_back(new DataToken<std::string>(STRING, s));
}
void Parser::addToken(double n){
	tokens.push_back(new DataToken<double>(NUMBER, n));
}

void Parser::addTokenFromStr(std::string& str_buffer){
	if (!str_buffer.empty()) {
		try	{
			size_t pos;
			double d = std::stod(str_buffer, &pos);
			if (pos < str_buffer.size()) {
				throw "couldn't convert entire number\n";
			}
			addToken(d);
		} catch (const std::invalid_argument& ia){
			addToken(str_buffer);
		} catch (const char* msg){
			addToken(str_buffer);
		}
		str_buffer = "";
	}
}

Parser::Parser(std::string input){
	std::string str_buffer = "";
	for (char& c : input) {
		if (std::isspace(c))
			continue;
		Token_type t = Token::parseTokenType(c);
		if (t == STRING) {
			str_buffer.append(1, c);
		} else {
			addTokenFromStr(str_buffer);
			addToken(t);
		}
	}
	addTokenFromStr(str_buffer);
}

bool Parser::match(Token_type tt){
	if (check(tt)) {
		if (!atEnd())
			current++;
		return true;
	}
	return false;
}

Token* Parser::consume(Token_type tt, const char* msg){
	if (match(tt))
		return prev();
	throw msg;
}

Expression* Parser::expression(Sheet* shp){
	Expression* expr = factor(shp);
	if (expr == NULL)
		throw "not enough arguments (+-)\n";
	while (match(MINUS) || match(PLUS)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = factor(shp);
		} catch (const char* msg){
			delete expr;
			throw msg;
		}
		if (rhs == NULL){
			delete expr;
			throw "not enough arguments (+-)\n";
		}
		expr = operandFromToken(operand, expr, rhs);
	}
	return expr;
}
Expression* Parser::factor(Sheet* shp){
	Expression* expr = unary(shp);
	if (expr == NULL)
		throw "not enough arguments (*/)\n";
	while (match(SLASH) || match(STAR)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = unary(shp);
		} catch (const char* msg) {
			delete expr;
			throw msg;
		}
		if (rhs == NULL){
			delete expr;
			return NULL;
		}
		expr = operandFromToken(operand, expr, rhs);
	}
	return expr;
}
Expression* Parser::unary(Sheet* shp){
	Expression* expr = NULL;
	if (match(MINUS) && (expr = unary(shp))) {
		return new Mult(new NumberExpr(-1), expr);
	} else if ((expr = function(shp))) {
		return expr;
	} else if ((expr = primary(shp))){
		return expr;
	}
	return expr;
}
Expression* Parser::function(Sheet* shp){
	size_t c = current;
	if (match(STRING)) {
		std::string fstr = dynamic_cast<DataToken<std::string>*>(prev())->getContent(); //bad cast
		FunctionName fname = FunctionExpr::parseFname(fstr);
		if (match(LEFT_BR)) {
			if (fname == INVALID)
				throw "invalid function name\n";
			CellRefExpr* c1 = cell(shp);
			consume(COLON, "invalid range in function\n");
			CellRefExpr* c2 = cell(shp);
			consume(RIGHT_BR, "mismatched brackets\n");
			if (c1!=NULL && c2!=NULL) {
				if (shp)
					return newFunctionExpr(fname, c1, c2, shp->getWidth());
				return newFunctionExpr(fname, c1, c2);
			} else {
				delete c1;
				delete c2;
			}
		}
	}
	current = c;
	return NULL;
}
Expression* Parser::primary(Sheet* shp){
	Expression* expr = NULL;
	if (match(NUMBER)) {
		try {
			DataToken<double>* nt = dynamic_cast<DataToken<double>*>(prev());
			return new NumberExpr(nt->getContent());
		} catch (const std::bad_cast& bc){
			throw "invalid number\n";
		}
	} else if (match(LEFT_BR)){
		expr = expression(shp);
		try {
			consume(RIGHT_BR, "mismatched brackets\n");
		} catch (const char* msg){
			delete expr;
			throw msg;
		}
		return expr;
	} else if ((expr = cell(shp))){
		return expr;
	}
	return NULL;
}
CellRefExpr* Parser::cell(Sheet* shp){
	CellRefExpr* expr = NULL;
	if(match(STRING)){
		try	{
			std::string cellstr = dynamic_cast<DataToken<std::string>*>(prev())->getContent();
			size_t i;
			for (i = 0; i < cellstr.size() && !std::isdigit(cellstr[i]); i++) {}
			std::string col = cellstr.substr(0, i);
			size_t pos;
			std::string numstr = cellstr.substr(i, cellstr.size()-i);
			int row = stoi(numstr, &pos);
			if (pos < numstr.size())
				throw "invalid cell\n";
			if (shp) {
				size_t cn = shp->colNumber(col);
				if (shp->checkRow(row) && shp->checkCol(cn))
					return new CellRefExpr(col, row, (*shp)[row-1] + cn-1);
			}
			return new CellRefExpr(col, row);
		} catch (const std::bad_cast& bc) {
		} catch (const std::invalid_argument& ia) {}
	}
	return expr;
}

Expression* Parser::parse(Sheet* shp){
	current = 0;
	Expression* expr = NULL;
	try{
		expr = expression(shp);
	} catch (const char* msg) {
		std::cout << msg;
		delete expr;
		expr = NULL;
	}
	return expr;
}

void Parser::parseTo(Sheet* shp, ExprPointer& ep){
	Expression* expr = parse(shp);
	if (expr) {
		ep = expr;
	}
}

Operator* operandFromToken(Token_type tt, Expression* lhs, Expression* rhs){
	switch (tt) {
		case PLUS:
			return new Add(lhs, rhs);
		case MINUS:
			return new Sub(lhs, rhs);
		case STAR:
			return new Mult(lhs, rhs);
		case SLASH:
			return new Div(lhs, rhs);
		default:
			return NULL;
	}
}
