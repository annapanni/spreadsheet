#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <vector>

#include "expression_core.hpp"
#include "sheet.hpp"
#include "exceptions.hpp"

class CellId {
	int colNum;
	int row;
public:
	int getColNum() const {return colNum;}
	int getRow() const {return row;}
	void setColNum(int c) {colNum = c;}
	void setRow(int r) {row = r;}
	CellId(std::string col, int row) : colNum(Sheet::colNumber(col)), row(row) {}
	CellId(std::string);
	std::string colLetter() const {return Sheet::colLetter(colNum);}
};

class CellRefExpr : public Expression {
	CellId cell;
	Sheet* sh;
	bool absCol;
	bool absRow;
public:
	CellRefExpr(std::string col, int row, Sheet* sh = NULL, bool absCol=false, bool absRow=false)
		: cell(CellId(col, row)), sh(sh), absCol(absCol), absRow(absRow) {}
	CellRefExpr(std::string str, Sheet* sh = NULL, bool absCol=false, bool absRow=false)
		: cell(CellId(str)), sh(sh), absCol(absCol), absRow(absRow) {}
	std::string getCol() const {return cell.colLetter();}
	int getRow() const {return cell.getRow();}
	Sheet* getSheet() const {return sh;}
	ExprPointer* getPtr() const {if (sh == NULL) throw eval_error("uninitialized cell");
		return sh->parseCell(cell.getColNum(), cell.getRow());}
	bool getAbsCol() const {return absCol;}
	bool getAbsRow() const {return absRow;}
	double eval() const;
	void checkCyclic(std::vector<Expression*>) const;
	std::string show() const {return (absCol?"$":"") + cell.colLetter() + (absRow?"$":"") + std::to_string(cell.getRow());}
	CellRefExpr* copy() const {return new CellRefExpr(*this);}
	void shift(int dx, int dy);
	void relocate(const void* shp) {sh = (Sheet*)shp;}
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
	void relocate(const void* shp) {topCell->relocate(shp); bottomCell->relocate(shp);}
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
	void relocate(const void* shp) {range.relocate(shp);}
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
	void relocate(const void* shp) {lhs->relocate(shp); rhs->relocate(shp);}
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
