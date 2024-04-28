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
				addToken(str_buffer);
			} else {
				addToken(d);
			}
		} catch (const std::invalid_argument& ia){
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

Parser& Parser::operator=(const Parser& p){
	if (&p != this) {
		for (size_t i = 0; i < tokens.size(); i++) {
			delete tokens[i];
		}
		tokens.resize(0);
		for (Token* t : p.tokens) {
			tokens.push_back(t->copy());
		}
		current = p.current;
	}
	return *this;
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
	throw syntax_error(msg);
}

Expression* Parser::expression(Sheet* shp){
	Expression* expr = factor(shp);
	if (expr == NULL)
		throw syntax_error("not enough arguments");
	while (match(MINUS) || match(PLUS)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = factor(shp);
		} catch (const std::runtime_error&){
			delete expr;
			throw;
		}
		if (rhs == NULL){
			delete expr;
			throw syntax_error("not enough arguments");
		}
		expr = operandFromToken(operand, expr, rhs);
	}
	return expr;
}
Expression* Parser::factor(Sheet* shp){
	Expression* expr = unary(shp);
	if (expr == NULL)
		throw syntax_error("not enough arguments");
	while (match(SLASH) || match(STAR)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = unary(shp);
		} catch (const std::runtime_error&) {
			delete expr;
			throw;
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
				throw syntax_error("invalid function name");
			CellRefExpr* c1 = NULL;
			CellRefExpr* c2 = NULL;
			try {
				c1 = cell(shp);
				consume(COLON, "invalid range in function");
				c2 = cell(shp);
				consume(RIGHT_BR, "mismatched brackets");
			} catch (const std::runtime_error&){
				delete c1;
				delete c2;
				throw;
			}
			if (c1!=NULL && c2!=NULL) {
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
			throw std::runtime_error("tokenization error");
		}
	} else if (match(LEFT_BR)){
		expr = expression(shp);
		try {
			consume(RIGHT_BR, "mismatched brackets");
		} catch (const std::runtime_error&){
			delete expr;
			throw;
		}
		return expr;
	} else if ((expr = cell(shp))){
		return expr;
	}
	return NULL;
}

CellRefExpr* Parser::cell(Sheet* shp){
	CellRefExpr* expr = NULL;
	bool absCol = false;
	if (match(DOLLAR))
		absCol = true;
	std::string colstr;
	if (match(STRING)) {
		try	{
			colstr = dynamic_cast<DataToken<std::string>*>(prev())->getContent();
		} catch (const std::bad_cast& bc) {throw std::runtime_error("tokenization error");}
		if (match(DOLLAR)) {//col and row are separated, row is absolute
			if (match(NUMBER)) {
				try	{
					int n = (int)dynamic_cast<DataToken<double>*>(prev())->getContent();
					return new CellRefExpr(colstr, n, shp, absCol, true);
				} catch (const std::bad_cast& bc) {throw std::runtime_error("tokenization error");}
			} else {
				throw syntax_error("invalid cell syntax");
			}
		} else {//row isn't absolute
			return new CellRefExpr(colstr, shp, absCol, false);
		}
	}
	return expr;
}

Expression* Parser::parse(Sheet* shp){
	current = 0;
	return expression(shp);
}

void Parser::parseTo(Sheet* shp, ExprPointer& ep){
	Expression* expr = parse(shp);
	if (expr) {
		ep = expr;
	}
}

std::string Parser::show(){
	std::string outp = "";
	for (Token*& t : tokens) {
		outp += t->show() + ", ";
	}
	return outp;
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
