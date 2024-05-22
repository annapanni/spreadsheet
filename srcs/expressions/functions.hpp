#ifndef FUNC_HPP
#define  FUNC_HPP

#include <string>
#include <vector>
#include <optional>

#include "expression_core.hpp"
#include "range.hpp"
#include "../exceptions.hpp"

///elérhető függvények nevei
enum FunctionName {
	AVG, SUM
};

///Tartományon elvégezhető függvénykifejezések absztrakt osztálya
class FunctionExpr : public Expression {
protected:
	Range range; ///<tartomány, melyen a függvény végrehajtódik
public:
	explicit FunctionExpr(Range r) : range(r) {} ///<konstruktor
	explicit FunctionExpr(CellRefExpr* topCell, CellRefExpr* bottomCell) : range(topCell, bottomCell) {} ///<konstruktor
	void checkCyclic(std::vector<Expression*>) const;
	void shift(int dx, int dy) {range.shift(dx, dy);}
	void relocate(Sheet* shp) {range.relocate(shp);}
	virtual ~FunctionExpr(){}
	///értelmezi a függvények neveit (case sensitive)
	static std::optional<FunctionName> parseFname(std::string name){
		if (name == "avg") return AVG;
		if (name == "sum") return SUM;
		return {};
	}
	///létrehoz egy megfelelő típusú függvényt a neve alapján
	static FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* topCell, CellRefExpr* bottomCell);
};

///Tartomány átlagát vevő függvény osztály
class AvgFunc : public FunctionExpr {
public:
	explicit AvgFunc(Range r) : FunctionExpr(r) {}
	explicit AvgFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "avg(" + range.show() + ")";}
	Expression* copy() const {return new AvgFunc(range);}
};

///Tartományt összegző függvény osztály
class SumFunc : public FunctionExpr {
public:
	explicit SumFunc(Range r) : FunctionExpr(r) {}
	explicit SumFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "sum(" + range.show() + ")";}
	Expression* copy() const {return new SumFunc(range);}
};


#endif
