#include "expression.hpp"

double CellRefExpr::eval() const {
	if (content == NULL)
		throw "uninitialized cell reference / cell reference out of bound\n";
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
	ExprPointer* bp = begin->getContent();
	ExprPointer* ep = end->getContent();
	if (bp == NULL || ep == NULL)
		throw "uninitialized cell reference in range\n";
	size_t db = 0;
	double sum = 0;
	//oszloponként járja be
	for (ExprPointer* colstart = bp; colstart + tableWidth <= ep; colstart++) {
		for (ExprPointer* cell = colstart ; cell <= ep; cell += tableWidth) {
			sum += (*cell)->eval();
			db++;
		}
	}
	return sum/(double)db;
}

double SumFunc::eval() const {
	ExprPointer* bp = begin->getContent();
	ExprPointer* ep = end->getContent();
	if (bp == NULL || ep == NULL)
		throw "uninitialized cell reference in range\n";
	size_t rangeWidth = (ep - bp) % tableWidth;
	double sum = 0;
	for (ExprPointer* row = bp; row <= ep-rangeWidth; row += tableWidth){
		for (ExprPointer* cell = row; cell <= row+rangeWidth; cell++){
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
