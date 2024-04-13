#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <vector>

#include "expression_core.hpp"
#include "sheet.hpp"

class CellId {
public:
	int colNum;
	int row;
	CellId(std::string col, int row) : colNum(Sheet::colNumber(col)), row(row) {}
	CellId(std::string);
	std::string colLetter() const {return Sheet::colLetter(colNum);}
};

class CellRefExpr : public Expression {
	CellId cell;
	Sheet* sh;
public:
	CellRefExpr(std::string col, int row, Sheet* sh = NULL) : cell(CellId(col, row)), sh(sh) {}
	CellRefExpr(std::string str, Sheet* sh = NULL) : cell(CellId(str)), sh(sh) {}
	std::string getCol() const {return cell.colLetter();}
	int getRow() const {return cell.row;}
	Sheet* getSheet() const {return sh;}
	ExprPointer* getPtr() const {if (sh == NULL) throw "uninitialized cell\n"; return sh->parseCell(cell.colNum, cell.row);}
	double eval() const;
	void checkCyclic(std::vector<Expression*>) const;
	std::string show() const {return cell.colLetter() + std::to_string(cell.row);}
	CellRefExpr* copy() const {return new CellRefExpr(*this);}
	void shift(int dx, int dy) {cell.row += dy; cell.colNum += dx;}
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
		ExprPointer& operator*() const {if (actCell==NULL) throw "empty iterator"; return *actCell;}
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
	static FunctionName parseFname(std::string name){
		if (name == "avg") return AVG;
		if (name == "sum") return SUM;
		return INVALID;
	}
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

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* topCell, CellRefExpr* bottomCell);

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
