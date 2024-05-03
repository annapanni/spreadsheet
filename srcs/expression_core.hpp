#ifndef EXPRESSION_CORE_HPP
#define EXPRESSION_CORE_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

class Sheet;

/**
*Kifejezések absztrakt alaposztálya.
*/
class Expression {
public:
	/**
	*rekuzívan kiértékeli a kifejezést.
	*kiértékelés közben eval_error típusa kivételt dobhat
	*/
	virtual double eval() const = 0;
	/**
	*ellenőrzi, tartalmaz-e a kifejezés ciklikus referenciát
	*amennyiben igen, eval_error kivételt dob
	@param ps - a kifejezésben korábban hivatkozott cellák, amire ismét hivatkozva körkörös hivatkozást kapunk
	*/
	virtual void checkCyclic(std::vector<Expression*>) const = 0;
	/**
	*rekuzívan kiértékeli a kifejezést, körkörös hivatkozásra is eval_error hibát dob
	@param ps - a kifejezésben korábban hivatkozott cellák, amire ismét hivatkozva körkörös hivatkozást kapunk
	*/
	virtual double safeEval(std::vector<Expression*> ps) const {checkCyclic(ps); return eval();}
	virtual std::string show() const = 0; ///<kifejezés megjelenítése std::string-ként
	virtual Expression* copy() const = 0; ///<dinamikusan foglalt memóriaterületen visszaadott másolat
	virtual void shift(int, int) {} ///<rekurzívan minden hivatkozást adott oszlop- és sorszámmal eltol
	virtual void relocate(Sheet*) {} ///<a kifejezésben található hivatkozások célpontját áthelyezi egy másik számolótáblára
	virtual ~Expression() {}; ///<destruktor
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

/**
*Valós számokat tároló kifejezés osztály
*/
class NumberExpr : public Expression {
	double value; ///<kifejezés értéke
public:
	NumberExpr(double v) : value(v) {} ///<konstruktor
	double eval() const {return value;} ///<kifejezés kiértékelése - érték visszaadása
	void checkCyclic(std::vector<Expression*>) const {} ///<körkörös hivatkozás keresése - ez az oszály sosem dob kivételt
	Expression* copy() const {return new NumberExpr(value);} ///<dinamikusan foglalt memóriaterületen visszaadott másolat
	std::string show() const {std::ostringstream ss; ss << value; return ss.str();} ///<szám megjelenítése std::string-ként
};

#endif
