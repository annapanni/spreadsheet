#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

enum Token_type {
	MINUS, PLUS, SLASH, STAR, LEFT_BR, RIGHT_BR, COLON,
	NUMBER, STRING
};

class Token {
protected:
	Token_type type;
public:
	Token() {}
	Token(Token_type t) : type(t) {}
	Token_type getType() const {return type;}

	void show() ;

	static Token_type parseTokenType(char c);
	virtual ~Token(){}
};

template <typename T>
class DataToken : public Token {
	T content;
public:
	DataToken(Token_type tt, T s) : Token(tt), content(s) {}
	T getContent() const {return content;}
};

#endif
