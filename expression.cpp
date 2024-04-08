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

double CellRefExpr::eval() const {
	return (*getPtr())->eval();
}

void CellRefExpr::checkCyclic(std::vector<Expression*> ps) const {
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
	topCell = new CellRefExpr(minCol, minRow, bg->getSheet());
	bottomCell = new CellRefExpr(maxCol, maxRow, bg->getSheet()); //assuming both cells are on the same sheet
	delete bg;
	delete ed;
}

Range& Range::operator=(const Range& r){
	if (&r != this){
		delete topCell;
		delete bottomCell;
		topCell = r.topCell->copy();
		bottomCell = r.bottomCell->copy();
	}
	return *this;
}

Range::iterator& Range::iterator::operator++() {//preinkremens
	if (actCell == NULL)
		throw "empty iterator";
	if (actCell + 1 <= actRow+rangeWidth) {
		++actCell;
	} else {
		actRow += tableWidth;
		actCell = actRow;
	}
	return *this;
}
Range::iterator Range::iterator::operator++(int) {//posztinkremens
	iterator tmp = *this;
	operator++();
	return tmp;
}
Range::iterator Range::begin() const{
	Sheet* sh = topCell->getSheet();
	ExprPointer* bp = topCell->getPtr();
	ExprPointer* ep = bottomCell->getPtr();
	size_t rangeWidth = (ep - bp - 1) % (int)sh->getWidth() + 1;
	return iterator(rangeWidth, sh->getWidth(), bp);
}
Range::iterator Range::end() const{
	Sheet* sh = topCell->getSheet();
	ExprPointer* bp = topCell->getPtr();
	ExprPointer* ep = bottomCell->getPtr();
	size_t rangeWidth = (ep - bp - 1) % (int)sh->getWidth() + 1;
	return iterator(rangeWidth, sh->getWidth(), ep-rangeWidth+sh->getWidth());
}

void FunctionExpr::checkCyclic(std::vector<Expression*> ps) const {
	Range::iterator cell;
	for (cell = range.begin(); cell != range.end(); cell++) {
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

FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* topCell, CellRefExpr* bottomCell){
	switch (fn) {
		case AVG:
			return new AvgFunc(topCell, bottomCell);
		case SUM:
			return new SumFunc(topCell, bottomCell);
		default:
			return NULL;
	}
}

double AvgFunc::eval() const {
	size_t db = 0;
	double sum = 0;
	Range::iterator cell;
	for (cell = range.begin(); cell != range.end(); cell++) {
		sum += (*cell)->eval();
		db++;
	}
	return sum/(double)db;
}

double SumFunc::eval() const {
	double sum = 0;
	Range::iterator cell;
	for (cell = range.begin(); cell != range.end(); cell++) {
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
