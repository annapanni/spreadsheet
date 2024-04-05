#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <vector>

#include "expression_core.hpp"
#include "sheet.hpp"

class CellId {
public:
	std::string col;
	int row;
	CellId(std::string col, int row) : col(col), row(row) {}
	CellId(std::string);
	size_t colNumber() const {return Sheet::colNumber(col);}
};

class CellRefExpr : public Expression {
	CellId cell;
	Sheet* sh;
public:
	CellRefExpr(std::string col, int row, Sheet* sh = NULL) : cell(CellId(col, row)), sh(sh) {}
	CellRefExpr(std::string str, Sheet* sh = NULL) : cell(CellId(str)), sh(sh) {}
	std::string getCol() const {return cell.col;}
	int getRow() const {return cell.row;}
	Sheet* getSheet() const {return sh;}
	ExprPointer* getPtr() const {if (sh == NULL) throw "uninitialized cell\n"; return sh->parseCell(cell.col, cell.row);}
	double eval();
	void checkCyclic(std::vector<Expression*>);
	std::string show() const {return cell.col + std::to_string(cell.row);}
	CellRefExpr* copy() const {return new CellRefExpr(*this);}
};

class Range {
	CellRefExpr* begin;
	CellRefExpr* end;		//exprected that begin and end are on the same sheet
	//initialized by beginIter:
	size_t rangeWidth;
	ExprPointer* iterRow;
	ExprPointer* iterCell;
public:
	Range(CellRefExpr* bg, CellRefExpr* ed);
	Range(const Range& r) : begin(r.begin->copy()), end(r.end->copy()) {}
	Range& operator=(const Range& r);
	void beginIter();
	ExprPointer* next();
	std::string show() const {return begin->show() + ":" + end->show();}
	~Range(){
		delete begin;
		delete end;
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
	FunctionExpr(CellRefExpr* begin, CellRefExpr* end) : range(begin, end) {}
	void checkCyclic(std::vector<Expression*>);
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
	AvgFunc(CellRefExpr* begin, CellRefExpr* end) : FunctionExpr(begin, end) {}
	double eval();
	std::string show() const {return "avg(" + range.show() + ")";}
	Expression* copy() const {return new AvgFunc(range);}
};

class SumFunc : public FunctionExpr {
public:
	SumFunc(Range r) : FunctionExpr(r) {}
	SumFunc(CellRefExpr* begin, CellRefExpr* end) : FunctionExpr(begin, end) {}
	double eval();
	std::string show() const {return "sum(" + range.show() + ")";}
	Expression* copy() const {return new SumFunc(range);}
};

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* begin, CellRefExpr* end);

class Operator : public Expression {
protected:
	Expression* lhs;
	Expression* rhs;
public:
	Operator(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) {}
	Operator(const Operator& op) : lhs(op.lhs->copy()), rhs(op.rhs->copy()) {}
	Operator& operator=(const Operator& op);
	void checkCyclic(std::vector<Expression*> ps) {lhs->checkCyclic(ps); rhs->checkCyclic(ps);}
	virtual ~Operator(){
		delete lhs;
		delete rhs;
	}
};

class Mult : public Operator {
public:
	Mult(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() {return lhs->eval() * rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "*" + rhs->show() + ")";}
	Expression* copy() const {return new Mult(lhs->copy(), rhs->copy());}
};

class Div : public Operator {
public:
	Div(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() {return lhs->eval() / rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "/" + rhs->show() + ")";}
	Expression* copy() const {return new Div(lhs->copy(), rhs->copy());}
};

class Add : public Operator {
public:
	Add(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() {return lhs->eval() + rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "+" + rhs->show() + ")";}
	Expression* copy() const {return new Add(lhs->copy(), rhs->copy());}
};

class Sub : public Operator {
public:
	Sub(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() {return lhs->eval() - rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "-" + rhs->show() + ")";}
	Expression* copy() const {return new Sub(lhs->copy(), rhs->copy());}
};

#endif
