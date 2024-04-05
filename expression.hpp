#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

class Expression {
public:
	virtual double eval() = 0;
	virtual void checkCyclic(std::vector<Expression*>) = 0;
	virtual double safeEval(std::vector<Expression*> ps) {checkCyclic(ps); return eval();}
	virtual std::string show() const = 0;
	virtual Expression* copy() const = 0;
	virtual ~Expression() {};
};

class ExprPointer {
	Expression* ep;
public:
	ExprPointer(Expression* p = NULL) : ep(p) {}
	ExprPointer(const ExprPointer& rhs) : ep(rhs.ep->copy()) {}
	Expression* operator*() const {return ep;}
	ExprPointer& operator=(const ExprPointer& rhs);
	Expression* operator->() const {return ep;}
	bool operator==(Expression* p) {return ep == p;}
	double evalMe() {return ep->safeEval({ep});}
	~ExprPointer() {delete ep;}
};

class NumberExpr : public Expression {
	double value;
public:
	NumberExpr(double v) : value(v) {}
	double eval() {return value;}
	void checkCyclic(std::vector<Expression*>) {}
	Expression* copy() const {return new NumberExpr(value);}
	std::string show() const {std::ostringstream ss; ss << value; return ss.str();}
};

class CellRefExpr : public Expression {
	std::string col;
	int row;
	ExprPointer* content = NULL;
public:
	CellRefExpr(std::string col, int row) : col(col), row(row) {}
	CellRefExpr(std::string col, int row, ExprPointer* e) : col(col), row(row), content(e) {}
	std::string getCol() const {return col;}
	int getRow() const {return row;}
	ExprPointer* getContent() const {return content;}
	double eval();
	void checkCyclic(std::vector<Expression*>);
	std::string show() const {return col + std::to_string(row);}
	CellRefExpr* copy() const {return new CellRefExpr(col, row, content);}
};

class Range {
	CellRefExpr* begin;
	CellRefExpr* end;
	size_t tableWidth = 0;
	//initialized by beginIter:
	size_t rangeWidth;
	ExprPointer* iterRow;
	ExprPointer* iterCell;
public:
	Range(CellRefExpr* bg, CellRefExpr* ed, size_t w=0);
	Range(const Range& r) : begin(r.begin->copy()), end(r.end->copy()), tableWidth(r.tableWidth) {}
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
	FunctionExpr(CellRefExpr* begin, CellRefExpr* end, size_t w=0) : range(begin, end, w) {}
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
	AvgFunc(CellRefExpr* begin, CellRefExpr* end, size_t w=0) : FunctionExpr(begin, end, w) {}
	double eval();
	std::string show() const {return "avg(" + range.show() + ")";}
	Expression* copy() const {return new AvgFunc(range);}
};

class SumFunc : public FunctionExpr {
public:
	SumFunc(Range r) : FunctionExpr(r) {}
	SumFunc(CellRefExpr* begin, CellRefExpr* end, size_t w=0) : FunctionExpr(begin, end, w) {}
	double eval();
	std::string show() const {return "sum(" + range.show() + ")";}
	Expression* copy() const {return new SumFunc(range);}
};

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* begin, CellRefExpr* end, size_t w = 0);

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
