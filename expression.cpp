#include "expression.hpp"

CellId::CellId(std::string cellstr){
	try	{
		size_t i;
		for (i = 0; i < cellstr.size() && !std::isdigit(cellstr[i]); i++) {}
		colNum = Sheet::colNumber(cellstr.substr(0, i));
		size_t pos;
		std::string numstr = cellstr.substr(i, cellstr.size()-i);
		row = stoi(numstr, &pos);
		if (pos < numstr.size())
			throw "invalid cell\n";
	} catch (const std::invalid_argument& ia) {throw "invalid cell\n";}
}


double CellRefExpr::eval() {
	return (*getPtr())->eval();
}

void CellRefExpr::checkCyclic(std::vector<Expression*> ps){
	for (Expression* expP : ps) {
		if (*getPtr() == expP) {
			throw "cyclic reference\n";
		}
	}
	ps.push_back(**getPtr());
	(*getPtr())->checkCyclic(ps);
}

Range::Range(CellRefExpr* bg, CellRefExpr* ed) {
	std::string bgCol = bg->getCol();
	std::string edCol = ed->getCol();
	int bgRow = bg->getRow();
	int edRow = ed->getRow();
	std::string minCol = bgCol < edCol ? bgCol : edCol;
	std::string maxCol = bgCol > edCol ? bgCol : edCol;
	int minRow = bgRow < edRow ? bgRow : edRow;
	int maxRow = bgRow > edRow ? bgRow : edRow;
	begin = new CellRefExpr(minCol, minRow, bg->getSheet());
	end = new CellRefExpr(maxCol, maxRow, bg->getSheet()); //assuming both cells are on the same sheet
	delete bg;
	delete ed;
}

Range& Range::operator=(const Range& r){
	if (&r != this){
		delete begin;
		delete end;
		begin = r.begin->copy();
		end = r.end->copy();
	}
	return *this;
}

void Range::beginIter() {
	Sheet* sh = begin->getSheet();
	ExprPointer* bp = begin->getPtr();
	ExprPointer* ep = end->getPtr();
	rangeWidth = (ep - bp - 1) % (int)sh->getWidth() + 1;
	iterCell = bp - 1;
	iterRow = bp;
}

ExprPointer* Range::next(){
	Sheet* sh = begin->getSheet();
	ExprPointer* ep = end->getPtr();
	if (iterCell+1 <= iterRow+rangeWidth) {
		return ++iterCell;
	} else if (iterRow + sh->getWidth() <= ep-rangeWidth){
		iterRow += sh->getWidth();
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

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* begin, CellRefExpr* end){
	switch (fn) {
		case AVG:
			return new AvgFunc(begin, end);
		case SUM:
			return new SumFunc(begin, end);
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
