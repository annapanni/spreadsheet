#include "token.hpp"

std::string Token::show(){
	switch (type) {
		case MINUS:
			return "minus";
		case PLUS:
			return "plus";
		case SLASH:
			return "slash";
		case STAR:
			return "star";
		case LEFT_BR:
			return "left br";
		case RIGHT_BR:
			return "right br";
		case COLON:
			return "colon";
		case NUMBER:
			return "number";
		default:
			return "string";
	}
}

Token_type Token::parseTokenType(char c){
	switch (c) {
		case '-':
			return MINUS;
		case '+':
			return PLUS;
		case '/':
			return SLASH;
		case '*':
			return STAR;
		case '(':
			return LEFT_BR;
		case ')':
			return RIGHT_BR;
		case ':':
			return COLON;
		default:
			return STRING;
	}
}
