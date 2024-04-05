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

void CellRefExpr::checkCyclic(std::vector<Expression*> ps){
	if (content == NULL)
		throw "uninitialized cell\n";
	for (Expression* expP : ps) {
		if (*content == expP) {
			throw "cyclic reference\n";
		}
	}
	ps.push_back(**content);
	(*content)->checkCyclic(ps);
}

Range::Range(CellRefExpr* bg, CellRefExpr* ed, size_t w) : begin(bg), end(ed), tableWidth(w) {}
/*
Range::Range(CellRefExpr* bg, CellRefExpr* ed, size_t w) : tableWidth(w) {
	//order the begin and end cells so they are in the upper left and lower right corner respectively
	std::string bgCol = bg->getCol();
	std::string edCol = ed->getCol();
	int bgRow = bg->getRow();
	int edRow = ed->getRow();
	if (bgCol <= edCol && bgRow <= edRow) {
		begin = bg;
		end = ed;
	} else if (bgCol >= edCol && bgRow >= edRow){
		begin = ed;
		end = bg;
	} else {//given cells are on the other diagonal
		if (bgCol <= edCol && bgRow > edRow) {
			CellRefExpr* tmp = bg;
			bg = ed;
			ed = tmp;
			bgCol = bg->getCol();
			edCol = ed->getCol();
			bgRow = bg->getRow();
			edRow = ed->getRow();
		}
		ExprPointer* bgp = bg->getContent();
		ExprPointer* edp = ed->getContent();
		std::cout << bgp->evalMe();
		std::cout << edp->evalMe() << "\n";
		ExprPointer* newBgp;
		ExprPointer* newEdp;
		if (bgp == NULL || edp == NULL) {
			newBgp = newEdp = NULL;
		} else {
			int rWidth = (int)tableWidth - (int)((edp - bgp - 1) % (int)w + 1);
			newBgp = bgp - rWidth;
			newEdp = edp + rWidth;
			std::cout << "switched\n";
			std::cout << bgp->evalMe();
			std::cout << edp->evalMe() << "\n";
			std::cout << newBgp->evalMe();
			std::cout << newEdp->evalMe() << "\n";
		}
		begin = new CellRefExpr(edCol, bgRow, newBgp);
		end = new CellRefExpr(bgCol, edRow, newEdp);
		delete bg;
		delete ed;
	}
}
*/

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
	rangeWidth = (ep - bp - 1) % (int)tableWidth + 1;
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

void FunctionExpr::checkCyclic(std::vector<Expression*> ps) {
	range.beginIter();
	ExprPointer* cell;
	while ((cell = range.next())) {
		for (Expression* exprP : ps) {
			if (*cell ==  exprP) {
				throw "cyclic reference\n";
			}
		}
		ps.push_back(**cell);
		(*cell)->checkCyclic(ps);
		ps.pop_back();
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

Operator& Operator::operator=(const Operator& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}
