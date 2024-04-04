#include "expression.hpp"

ExprPointer& ExprPointer::operator=(const ExprPointer& rhs) {
	if (&rhs != this) {
		delete ep;
		ep = rhs.ep->copy();
	}
	return *this;
}

double CellRefExpr::eval() {
	if (content == NULL)
		throw "uninitialized cell reference / cell reference out of bound\n";
	return (*content)->eval();
}


/*
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
}*/

Range& Range::operator=(const Range& r){
	if (&r != this){
		delete begin;
		delete end;
		begin = r.begin->copy();
		end = r.end->copy();
		tableWidth = r.tableWidth;
	}
	return *this;
}

void Range::beginIter() {
	ExprPointer* bp = begin->getContent();
	ExprPointer* ep = end->getContent();
	if (bp == NULL || ep == NULL)
		throw "uninitialized cell reference in range\n";
	rangeWidth = (ep - bp) % tableWidth;
	iterCell = bp - 1;
	iterRow = bp;
}

ExprPointer* Range::next(){
	ExprPointer* ep = end->getContent();
	if (iterCell+1 <= iterRow+rangeWidth) {
		return ++iterCell;
	} else if (iterRow + tableWidth <= ep-rangeWidth){
		iterRow += tableWidth;
		iterCell = iterRow;
		return iterCell;
	} else {
		return NULL;//end of iteration
	}
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

double AvgFunc::eval() {
	size_t db = 0;
	double sum = 0;
	range.beginIter();
	ExprPointer* cell;
	while ((cell = range.next())) {
		sum += (*cell)->eval();
		db++;
	}
	return sum/(double)db;
}

double SumFunc::eval() {
	double sum = 0;
	range.beginIter();
	ExprPointer* cell;
	while ((cell = range.next())) {
		sum += (*cell)->eval();
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
