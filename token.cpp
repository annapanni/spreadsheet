#include "token.hpp"

void Token::show(){
	switch (type) {
		case MINUS:
			std::cout << "minus"; break;
		case PLUS:
			std::cout << "plus"; break;
		case SLASH:
			std::cout << "slash"; break;
		case STAR:
			std::cout << "star"; break;
		case LEFT_BR:
			std::cout << "left br"; break;
		case RIGHT_BR:
			std::cout << "right br"; break;
		case COLON:
			std::cout << "colon"; break;
		case NUMBER:
			std::cout << "number"; break;
		default:
			std::cout << "string";
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
