#ifndef EXPRESSION_CORE_HPP
#define EXPRESSION_CORE_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

class Sheet;

class Expression {
public:
	virtual double eval() const = 0;
	virtual void checkCyclic(std::vector<Expression*>) const = 0;
	virtual double safeEval(std::vector<Expression*> ps) const {checkCyclic(ps); return eval();}
	virtual std::string show() const = 0;
	virtual Expression* copy() const = 0;
	virtual void shift(int, int) {}
	virtual void relocate(Sheet*) {}
	virtual ~Expression() {};
};

class ExprPointer {
	Expression* ep;
public:
	ExprPointer(Expression* p = NULL) : ep(p) {}
	ExprPointer(const ExprPointer& rhs) : ep(rhs.ep->copy()) {}
	operator Expression*() const {return ep;}
	ExprPointer& operator=(const ExprPointer& rhs) {
		if (&rhs != this) {
			delete ep;
			ep = rhs.ep->copy();
		}
		return *this;
	}
	bool operator==(const ExprPointer& rhs) const {return ep == rhs.ep;}
	Expression* operator->() const {return ep;}
	bool operator==(Expression* p) {return ep == p;}
	double evalMe() {return ep->safeEval({ep});}
	~ExprPointer() {delete ep;}
};

class NumberExpr : public Expression {
	double value;
public:
	NumberExpr(double v) : value(v) {}
	double eval() const {return value;}
	void checkCyclic(std::vector<Expression*>) const {}
	Expression* copy() const {return new NumberExpr(value);}
	std::string show() const {std::ostringstream ss; ss << value; return ss.str();}
};

#endif
