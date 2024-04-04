#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <sstream>
#include <iostream>

class Expression {
public:
	virtual double eval() const = 0;
	virtual std::string show() const = 0;
	virtual Expression* copy() const = 0;
	virtual ~Expression() {};
};

class ExprPointer {
	Expression* ep;
public:
	ExprPointer(Expression* p = NULL) : ep(p) {}
	ExprPointer(const ExprPointer& rhs) : ep(rhs.ep->copy()) {}
	Expression* getPtr() const {return ep;}
	ExprPointer& operator=(const ExprPointer& rhs) {
		if (&rhs != this) {
			delete ep;
			ep = rhs.ep->copy();
		}
		return *this;
	}
	Expression* operator->() const {return ep;}
	~ExprPointer() {delete ep;}
};

class NumberExpr : public Expression {
	double value;
public:
	NumberExpr(double v) : value(v) {}
	double eval() const {return value;}
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
	ExprPointer* getContent() const {return content;}
	double eval() const;
	std::string show() const {return col + std::to_string(row);}
	CellRefExpr* copy() const {return new CellRefExpr(col, row, content);}
};

enum FunctionName {
	INVALID, AVG, SUM
};

class FunctionExpr : public Expression {
protected:
	CellRefExpr* begin;
	CellRefExpr* end;
	size_t tableWidth = 0;
public:
	FunctionExpr(CellRefExpr* begin, CellRefExpr* end) : begin(begin), end(end) {}
	FunctionExpr(CellRefExpr* begin, CellRefExpr* end, size_t w) : begin(begin), end(end), tableWidth(w) {}
	static FunctionName parseFname(std::string name){
		if (name == "avg") return AVG;
		if (name == "sum") return SUM;
		return INVALID;
	}
	virtual ~FunctionExpr(){
		delete begin;
		delete end;
	}
};

class AvgFunc : public FunctionExpr {
public:
	AvgFunc(CellRefExpr* begin, CellRefExpr* end) : FunctionExpr(begin, end) {}
	AvgFunc(CellRefExpr* begin, CellRefExpr* end, size_t w) : FunctionExpr(begin, end, w) {}
	double eval() const;
	std::string show() const {return "avg(" + begin->show() + ":" + end->show() + ")";}
	Expression* copy() const {return new AvgFunc(begin->copy(), end->copy(), tableWidth);}
};

class SumFunc : public FunctionExpr {
public:
	SumFunc(CellRefExpr* begin, CellRefExpr* end) : FunctionExpr(begin, end) {}
	SumFunc(CellRefExpr* begin, CellRefExpr* end, size_t w) : FunctionExpr(begin, end, w) {}
	double eval() const;
	std::string show() const {return "sum(" + begin->show() + ":" +	end->show() + ")";}
	Expression* copy() const {return new SumFunc(begin->copy(), end->copy(), tableWidth);}
};

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* begin, CellRefExpr* end, size_t w = 0);

class Operand : public Expression {
protected:
	Expression* lhs;
	Expression* rhs;
public:
	Operand(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) {}
	virtual ~Operand(){
		delete lhs;
		delete rhs;
	}
};

class Mult : public Operand {
public:
	Mult(Expression* lhs, Expression* rhs) : Operand(lhs, rhs) {}
	Mult(const Mult& op) : Operand (op.lhs->copy(), op.rhs->copy()){}
	Mult& operator=(const Mult& op);
	double eval() const {return lhs->eval() * rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "*" + rhs->show() + ")";}
	Expression* copy() const {return new Mult(lhs->copy(), rhs->copy());}
};

class Div : public Operand {
public:
	Div(Expression* lhs, Expression* rhs) : Operand(lhs, rhs) {}
	Div(const Div& op) : Operand (op.lhs->copy(), op.rhs->copy()){}
	Div& operator=(const Div& op);
	double eval() const {return lhs->eval() / rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "/" + rhs->show() + ")";}
	Expression* copy() const {return new Div(lhs->copy(), rhs->copy());}
};

class Add : public Operand {
public:
	Add(Expression* lhs, Expression* rhs) : Operand(lhs, rhs) {}
	Add(const Add& op) : Operand (op.lhs->copy(), op.rhs->copy()){}
	Add& operator=(const Add& op);
	double eval() const {return lhs->eval() + rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "+" + rhs->show() + ")";}
	Expression* copy() const {return new Add(lhs->copy(), rhs->copy());}
};

class Sub : public Operand {
public:
	Sub(Expression* lhs, Expression* rhs) : Operand(lhs, rhs) {}
	Sub(const Sub& op) : Operand (op.lhs->copy(), op.rhs->copy()){}
	Sub& operator=(const Sub& op);
	double eval() const {return lhs->eval() - rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "-" + rhs->show() + ")";}
	Expression* copy() const {return new Sub(lhs->copy(), rhs->copy());}
};

#endif
