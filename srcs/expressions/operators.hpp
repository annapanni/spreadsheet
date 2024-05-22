#ifndef OPS_HPP
#define  OPS_HPP

#include <string>
#include <vector>

#include "expression_core.hpp"
#include "../sheet.hpp"
#include "../exceptions.hpp"
#include "../token.hpp"


///bináris műveleteket reprezentáló absztrakt osztály
class Operator : public Expression {
protected:
	Expression* lhs; ///<bal oldali operandus
	Expression* rhs; ///<jobb oldali operandus
public:
	///konstruktor
	/**
	@param lhs - bal oldali operandus
	@param rhs - jobb oldali operandus
	*/
	explicit Operator(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) {}
	Operator(const Operator& op) : lhs(op.lhs->copy()), rhs(op.rhs->copy()) {} ///<másoló konstruktor
	Operator& operator=(const Operator& op); ///<értékadás operátor
	void checkCyclic(std::vector<Expression*> prevs) const {lhs->checkCyclic(prevs); rhs->checkCyclic(prevs);}
	void shift(int dx, int dy) {lhs->shift(dx, dy); rhs->shift(dx, dy);}
	void relocate(Sheet* shp) {lhs->relocate(shp); rhs->relocate(shp);}
	///felszabadítja az operandusait
	virtual ~Operator(){
		delete lhs;
		delete rhs;
	}
	///adott tokentípusnak megfelelő műveletet hoz létre
	static Operator* operandFromToken(Token_type ttype, Expression* lhs, Expression* rhs);
};

///Szorzás műveletet reprezentáló osztály
class Mult : public Operator {
public:
	explicit Mult(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() * rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "*" + rhs->show() + ")";}
	Expression* copy() const {return new Mult(lhs->copy(), rhs->copy());}
};

///Osztás műveletet reprezentáló osztály
class Div : public Operator {
public:
	explicit Div(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() / rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "/" + rhs->show() + ")";}
	Expression* copy() const {return new Div(lhs->copy(), rhs->copy());}
};

///Összeadás műveletet reprezentáló osztály
class Add : public Operator {
public:
	explicit Add(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() + rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "+" + rhs->show() + ")";}
	Expression* copy() const {return new Add(lhs->copy(), rhs->copy());}
};

///Kivonás műveletet reprezentáló osztály
class Sub : public Operator {
public:
	explicit Sub(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() - rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "-" + rhs->show() + ")";}
	Expression* copy() const {return new Sub(lhs->copy(), rhs->copy());}
};

#endif
