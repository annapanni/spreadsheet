#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <vector>

#include "expression_core.hpp"
#include "sheet.hpp"
#include "exceptions.hpp"

/**
*Cellát azonosító sor- és oszlopadat eltárolására szolgáló osztály.
*/
class CellId {
	int colNum; ///<oszlop sorszáma 1-től indexelve
	int row; ///<sorszám 1-től indexelve
public:
	CellId(std::string col, int row) : colNum(Sheet::colNumber(col)), row(row) {}
			///<konstruktor oszlopjelölő betű és sorszám megadásával
	CellId(std::string); ///<konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel
	int getColNum() const {return colNum;} ///<oszlopszám lekérdezése
	int getRow() const {return row;} ///<sorszám lekérdezése
	void setColNum(int c) {colNum = c;} ///<oszlopszám beállítása
	void setRow(int r) {row = r;} ///<sorszám beállítása
	std::string colLetter() const {return Sheet::colLetter(colNum);} ///<oszlopbetű lekérdezése
};

/**
*Cellahivatkozást reprezentáló kifejezés osztály.
*/
class CellRefExpr : public Expression {
	CellId cell; ///<cellát azonosító sor- és oszlopadat
	Sheet* sh; ///<tábla, amelyre a hivatkozás mutat
	bool absCol; ///<oszlopát tekintve abszolút-e a hivatkozás
	bool absRow; ///<sorát tekintve abszolút-e a hivatkozás
public:
	CellRefExpr(std::string col, int row, Sheet* sh = NULL, bool absCol=false, bool absRow=false)
		: cell(CellId(col, row)), sh(sh), absCol(absCol), absRow(absRow) {}
		///<konstruktor oszlopjelölő betű és sorszám megadásával és privát adattagok inicializálásával
	CellRefExpr(std::string str, Sheet* sh = NULL, bool absCol=false, bool absRow=false)
		: cell(CellId(str)), sh(sh), absCol(absCol), absRow(absRow) {}
		///<konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel és privát adattagok inicializálásával
	std::string getCol() const {return cell.colLetter();} ///<oszlopbetű lekérdezése
	int getRow() const {return cell.getRow();} ///<sorszám lekérdezése
	Sheet* getSheet() const {return sh;} ///<hivatkozás által mutatott tábla lekérdezése
	ExprPointer* getPtr() const {if (sh == NULL) throw eval_error("uninitialized cell");
		return sh->parseCell(cell.getColNum(), cell.getRow());}
		///<hivatkozás által mutatott cellára mutató pointer lekérdezése
	bool getAbsCol() const {return absCol;} ///<oszlop abszolút voltának lekérdezése
	bool getAbsRow() const {return absRow;} ///<sor abszolút voltának lekérdezése
	double eval() const; ///<hivatkozás által mutatott cella kiértékelése
	void checkCyclic(std::vector<Expression*>) const;
		///<ellenőrzi, tartalmaz-e a kifejezés ciklikus referenciát ld. Expression::checkCyclic
	std::string show() const {return (absCol?"$":"") + cell.colLetter() + (absRow?"$":"") + std::to_string(cell.getRow());}
		 ///<kifejezés megjelenítése std::string-ként
	CellRefExpr* copy() const {return new CellRefExpr(*this);} ///<dinamikusan foglalt memóriaterületen visszaadott másolat
	/**
	*Eltolja a hivatkozást adott sorral és oszloppal, amennyiben a sor/oszlop nem abszolút
	@param dx - sor eltolásának mértéke (akár negatív)
	@param dy - oszlop eltolásának mértéke (akár negatív)
	*/
	void shift(int dx, int dy);
	void relocate(Sheet* shp) {sh = shp;} ///<a kifejezésben található hivatkozások célpontját áthelyezi egy másik számolótáblára
};

class Range {
	CellRefExpr* topCell;
	CellRefExpr* bottomCell;		//expected that begin and end are on the same sheet
public:
	Range(CellRefExpr* bg, CellRefExpr* ed);
	Range(const Range& r) : topCell(r.topCell->copy()), bottomCell(r.bottomCell->copy()) {}
	Range& operator=(const Range& r);
	class iterator {
		size_t rangeWidth;
		size_t tableWidth;
		ExprPointer* actRow;
		ExprPointer* actCell;
	public:
		iterator() : rangeWidth(0), tableWidth(0), actRow(NULL), actCell(NULL) {}
		iterator(size_t rw, size_t tw, ExprPointer* bp)
			: rangeWidth(rw), tableWidth(tw), actRow(bp), actCell(bp) {}
		ExprPointer& operator*() const {if (actCell==NULL) throw std::runtime_error("empty iterator"); return *actCell;}
		ExprPointer* operator->() const {return actCell;}
		bool operator==(const ExprPointer* ep) const {return actCell == ep;}
		bool operator==(const iterator& it) const {return actCell == it.actCell;}
		bool operator!=(const iterator& it) const {return actCell != it.actCell;}
		iterator& operator++();
		iterator operator++(int);
	};
	iterator begin() const;
	iterator end() const;
	std::string show() const {return topCell->show() + ":" + bottomCell->show();}
	void shift(int dx, int dy) {topCell->shift(dx, dy); bottomCell->shift(dx, dy);}
	void relocate(Sheet* shp) {topCell->relocate(shp); bottomCell->relocate(shp);}
	~Range(){
		delete topCell;
		delete bottomCell;
	}
};

enum FunctionName {
	INVALID, AVG, SUM
};

class FunctionExpr : public Expression {
protected:
	Range range;
public:
	FunctionExpr(Range r) : range(r) {}
	FunctionExpr(CellRefExpr* topCell, CellRefExpr* bottomCell) : range(topCell, bottomCell) {}
	void checkCyclic(std::vector<Expression*>) const;
	void shift(int dx, int dy) {range.shift(dx, dy);}
	void relocate(Sheet* shp) {range.relocate(shp);}
	static FunctionName parseFname(std::string name){
		if (name == "avg") return AVG;
		if (name == "sum") return SUM;
		return INVALID;
	}
	static FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* topCell, CellRefExpr* bottomCell);
	virtual ~FunctionExpr(){}
};

class AvgFunc : public FunctionExpr {
public:
	AvgFunc(Range r) : FunctionExpr(r) {}
	AvgFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "avg(" + range.show() + ")";}
	Expression* copy() const {return new AvgFunc(range);}
};

class SumFunc : public FunctionExpr {
public:
	SumFunc(Range r) : FunctionExpr(r) {}
	SumFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "sum(" + range.show() + ")";}
	Expression* copy() const {return new SumFunc(range);}
};

class Operator : public Expression {
protected:
	Expression* lhs;
	Expression* rhs;
public:
	Operator(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) {}
	Operator(const Operator& op) : lhs(op.lhs->copy()), rhs(op.rhs->copy()) {}
	Operator& operator=(const Operator& op);
	void checkCyclic(std::vector<Expression*> ps) const {lhs->checkCyclic(ps); rhs->checkCyclic(ps);}
	void shift(int dx, int dy) {lhs->shift(dx, dy); rhs->shift(dx, dy);}
	void relocate(Sheet* shp) {lhs->relocate(shp); rhs->relocate(shp);}
	virtual ~Operator(){
		delete lhs;
		delete rhs;
	}
};

class Mult : public Operator {
public:
	Mult(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() * rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "*" + rhs->show() + ")";}
	Expression* copy() const {return new Mult(lhs->copy(), rhs->copy());}
};

class Div : public Operator {
public:
	Div(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() / rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "/" + rhs->show() + ")";}
	Expression* copy() const {return new Div(lhs->copy(), rhs->copy());}
};

class Add : public Operator {
public:
	Add(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() + rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "+" + rhs->show() + ")";}
	Expression* copy() const {return new Add(lhs->copy(), rhs->copy());}
};

class Sub : public Operator {
public:
	Sub(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() - rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "-" + rhs->show() + ")";}
	Expression* copy() const {return new Sub(lhs->copy(), rhs->copy());}
};

#endif
