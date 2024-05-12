#include <cctype>
#include "parser.hpp"

void Parser::addToken(Token_type type){
	tokens.push_back(new Token(type));
}
void Parser::addToken(std::string s){
	tokens.push_back(new DataToken<std::string>(STRING, s));
}
void Parser::addToken(double num){
	tokens.push_back(new DataToken<double>(NUMBER, num));
}

void Parser::addTokenFromStr(std::string& str_buffer){
	if (!str_buffer.empty()) {
		try	{
			size_t pos;
			double num = std::stod(str_buffer, &pos);
			if (pos < str_buffer.size()) {
				addToken(str_buffer);
			} else {
				addToken(num);
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
		Token_type type = Token::parseTokenType(c);
		if (type == STRING) {
			str_buffer.append(1, c);
		} else {
			addTokenFromStr(str_buffer);
			addToken(type);
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

bool Parser::match(Token_type ttype){
	if (check(ttype)) {
		if (!atEnd())
			current++;
		return true;
	}
	return false;
}

Token* Parser::consume(Token_type ttype, const char* msg){
	if (match(ttype))
		return prev();
	throw syntax_error(msg);
}

Expression* Parser::expression(Sheet* shptr){
	Expression* expr = factor(shptr);
	if (expr == nullptr)
		throw syntax_error("not enough arguments");
	while (match(MINUS) || match(PLUS)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = factor(shptr);
		} catch (const std::runtime_error&){
			delete expr;
			throw;
		}
		if (rhs == nullptr){
			delete expr;
			throw syntax_error("not enough arguments");
		}
		expr = Operator::operandFromToken(operand, expr, rhs);
	}
	return expr;
}

Expression* Parser::factor(Sheet* shptr){
	Expression* expr = unary(shptr);
	if (expr == nullptr)
		throw syntax_error("not enough arguments");
	while (match(SLASH) || match(STAR)){
		Token_type operand = prev()->getType();
		Expression* rhs;
		try {
			rhs = unary(shptr);
		} catch (const std::runtime_error&) {
			delete expr;
			throw;
		}
		if (rhs == nullptr){
			delete expr;
			return nullptr;
		}
		expr = Operator::operandFromToken(operand, expr, rhs);
	}
	return expr;
}

Expression* Parser::unary(Sheet* shptr){
	Expression* expr = nullptr;
	if (match(MINUS) && (expr = unary(shptr))) {
		return new Mult(new NumberExpr(-1), expr);
	} else if ((expr = function(shptr))) {
		return expr;
	} else if ((expr = primary(shptr))){
		return expr;
	}
	return expr;
}

Expression* Parser::function(Sheet* shptr){
	size_t c = current;
	if (match(STRING)) {
		std::string fnameStr = dynamic_cast<DataToken<std::string>*>(prev())->getContent(); //bad cast
		std::optional<FunctionName> fname = FunctionExpr::parseFname(fnameStr);
		if (match(LEFT_BR)) {
			if (!fname)
				throw syntax_error("invalid function name");
			CellRefExpr* c1 = nullptr;
			CellRefExpr* c2 = nullptr;
			try {
				c1 = cell(shptr);
				consume(COLON, "invalid range in function");
				c2 = cell(shptr);
				consume(RIGHT_BR, "mismatched brackets");
			} catch (const std::runtime_error&){
				delete c1;
				delete c2;
				throw;
			}
			if (c1!=nullptr && c2!=nullptr) {
				return FunctionExpr::newFunctionExpr(fname.value(), c1, c2);
			} else {
				delete c1;
				delete c2;
			}
		}
	}
	current = c;
	return nullptr;
}

Expression* Parser::primary(Sheet* shptr){
	Expression* expr = nullptr;
	if (match(NUMBER)) {
		try {
			DataToken<double>* numToken = dynamic_cast<DataToken<double>*>(prev());
			return new NumberExpr(numToken->getContent());
		} catch (const std::bad_cast&){
			throw std::runtime_error("tokenization error");
		}
	} else if (match(LEFT_BR)){
		expr = expression(shptr);
		try {
			consume(RIGHT_BR, "mismatched brackets");
		} catch (const std::runtime_error&){
			delete expr;
			throw;
		}
		return expr;
	} else if ((expr = cell(shptr))){
		return expr;
	}
	return nullptr;
}

CellRefExpr* Parser::cell(Sheet* shptr){
	CellRefExpr* expr = nullptr;
	bool absCol = false;
	if (match(DOLLAR))
		absCol = true;
	std::string colstr;
	if (match(STRING)) {
		try	{
			colstr = dynamic_cast<DataToken<std::string>*>(prev())->getContent();
		} catch (const std::bad_cast&) {throw std::runtime_error("tokenization error");}
		if (match(DOLLAR)) {//col and row are separated, row is absolute
			if (match(NUMBER)) {
				try	{
					int n = (int)dynamic_cast<DataToken<double>*>(prev())->getContent();
					return new CellRefExpr(colstr, n, shptr, absCol, true);
				} catch (const std::bad_cast& bc) {throw std::runtime_error("tokenization error");}
			} else {
				throw syntax_error("invalid cell syntax");
			}
		} else {//row isn't absolute
			return new CellRefExpr(colstr, shptr, absCol, false);
		}
	}
	return expr;
}

Expression* Parser::parse(Sheet* shptr){
	current = 0;
	return expression(shptr);
}

void Parser::parseTo(Sheet* shptr, ExprPointer& target){
	Expression* expr = parse(shptr);
	if (expr) {
		target = expr;
	}
}

std::string Parser::show(){
	std::string outp = "";
	for (Token*& t : tokens) {
		outp += t->show() + ", ";
	}
	return outp;
}
