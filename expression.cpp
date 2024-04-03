#include "expression.hpp"

double CellRefExpr::eval() const {
	if (content == NULL)
		throw "uninitialized cell reference / cell reference out of bound";
	return (*content)->eval();
}

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* begin, CellRefExpr* end, size_t w){
	switch (fn) {
		case AVG:
			return new AvgFunc(begin, end, w);
		case SUM:
			return new SumFunc(begin, end, w);
		default:
			return NULL;
	}
}

double AvgFunc::eval() const {
	Expression** bp = begin->getContent();
	Expression** ep = end->getContent();
	if (bp == NULL || ep == NULL)
		throw "uninitialized cell reference in range";
	size_t db = 0;
	double sum = 0;
	//oszloponként járja be
	for (Expression** colstart = bp; colstart + tableWidth <= ep; colstart++) {
		for (Expression** cell = colstart ; cell <= ep; cell += tableWidth) {
			sum += (*cell)->eval();
			db++;
		}
	}
	return sum/(double)db;
}

double SumFunc::eval() const {
	if (begin->getContent() == NULL || end->getContent() == NULL)
		throw "uninitialized cell reference in range";
	Expression** bp = begin->getContent();
	Expression** ep = end->getContent();
	double sum = 0;
	//oszloponként járja be
	for (Expression** colstart = bp; colstart + tableWidth <= ep; colstart++) {
		for (Expression** cell = colstart ; cell <= ep; cell += tableWidth) {
			sum += (*cell)->eval();
		}
	}
	return sum;
}

Mult& Mult::operator=(const Mult& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}

Div& Div::operator=(const Div& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}

Add& Add::operator=(const Add& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}

Sub& Sub::operator=(const Sub& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}
