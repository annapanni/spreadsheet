#include "operators.hpp"
#include "../exceptions.hpp"


Operator& Operator::operator=(const Operator& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}

Operator* Operator::operandFromToken(Token_type ttype, Expression* lhs, Expression* rhs){
	switch (ttype) {
		case PLUS:
			return new Add(lhs, rhs);
		case MINUS:
			return new Sub(lhs, rhs);
		case STAR:
			return new Mult(lhs, rhs);
		case SLASH:
			return new Div(lhs, rhs);
		default:
			return nullptr;
	}
}
