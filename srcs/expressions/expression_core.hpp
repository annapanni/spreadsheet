#ifndef EXPRESSION_CORE_HPP
#define EXPRESSION_CORE_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

class Sheet;

///Kifejezések absztrakt alaposztálya.
class Expression {
public:
	///rekurzívan kiértékeli a kifejezést.
	/**kiértékelés közben eval_error típusa kivételt dobhat*/
	virtual double eval() const = 0;
	///ellenőrzi, tartalmaz-e a kifejezés ciklikus referenciát
	/**amennyiben igen, eval_error kivételt dob
	@param prevs - a kifejezésben korábban hivatkozott cellák, amire ismét hivatkozva körkörös hivatkozást kapunk
	*/
	virtual void checkCyclic(std::vector<Expression*>) const = 0;
	///rekurzívan kiértékeli a kifejezést, körkörös hivatkozásra is eval_error hibát dob
  /**
	@param prevs - a kifejezésben korábban hivatkozott cellák, amire ismét hivatkozva körkörös hivatkozást kapunk
	*/
	double safeEval(std::vector<Expression*> prevs) const {checkCyclic(prevs); return eval();}
	virtual std::string show() const = 0; ///<kifejezés megjelenítése std::string-ként
	virtual Expression* copy() const = 0; ///<dinamikusan foglalt memóriaterületen visszaadott másolat
	virtual void shift(int, int) {} ///<rekurzívan minden hivatkozást adott oszlop- és sorszámmal eltol
	virtual void relocate(Sheet*) {} ///<a kifejezésben található hivatkozások célpontját áthelyezi egy másik számolótáblára
	virtual ~Expression() {}; ///<destruktor
};

///A kifejezésekre mutató pointerek wrapper osztálya.
/**
*Az Expression absztrakt osztályból származtatott osztályok példányait heterogén
kollekciókban pointerként tudjuk tárolni. Azonban ilyenkor ügyelni kell a dinamikusan
foglalt memóriaterületekre, a pointerek másolására, értékadásra. Az ExprPointer
osztály ezt hivatott lekezelni, azáltal, hogy a másolja és felszabadítja a pointereket,
de az eredeti funkciójukat is megtartja. Így minden kifejezést gyakorlatilag sima
osztálypéldányként tudunk kezelni (pointer helyett).
*/
class ExprPointer {
	Expression* content; ///<az osztály által becsomagolt pointer
public:
	ExprPointer(Expression* p = nullptr) : content(p) {} ///<konstruktor pointer inicializálásával
	ExprPointer(const ExprPointer& rhs) : content(rhs.content->copy()) {} ///<másoló konstruktor
	operator Expression*() const {return content;} ///<castolás Expression*-ra
	ExprPointer& operator=(const ExprPointer& rhs) {
		if (&rhs != this) {
			delete content;
			content = rhs.content->copy();
		}
		return *this;
	} ///<értékadás a másik kifejezés rekurzív másolásával
	bool operator==(const ExprPointer& rhs) const {return content == rhs.content;} ///<egyenlőség másik ExprPointer-el
	bool operator==(Expression* p) {return content == p;} ///<egyenlőség Expression*-al
	Expression* operator->() const {return content;} ///<becsomagolt pointer adatainak és függvényeinek elérése nyíllal
	double evalMe() {return content->safeEval({content});}
		///<kiértékeli az adott kifejezést úgy, hogy, a körkörös hivatkozások keresése tőle indul
	~ExprPointer() {delete content;} ///<felszabadítja a pointert
};

///Valós számokat tároló kifejezés osztály
class NumberExpr : public Expression {
	double value; ///<kifejezés értéke
public:
	NumberExpr(double v) : value(v) {} ///<konstruktor
	double eval() const {return value;} ///<kifejezés kiértékelése - érték visszaadása
	void checkCyclic(std::vector<Expression*>) const {}
	Expression* copy() const {return new NumberExpr(value);}
	std::string show() const {std::ostringstream ss; ss << value; return ss.str();}
};

#endif
