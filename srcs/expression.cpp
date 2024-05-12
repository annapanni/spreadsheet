#include "expression.hpp"

//CellId fuctions --------------------------------------------------------------
CellId::CellId(std::string cellstr){
	try	{
		size_t i;
		for (i = 0; i < cellstr.size() && !std::isdigit(cellstr[i]); i++) {}
		colNum = Sheet::colNumber(cellstr.substr(0, i));
		size_t pos;
		std::string numstr = cellstr.substr(i, cellstr.size()-i);
		row = stoi(numstr, &pos);
		if (pos < numstr.size())
			throw syntax_error("invalid cell");
	} catch (const std::invalid_argument& ia) {throw syntax_error("invalid cell");}
}

//CellRefExpr fuctions ---------------------------------------------------------
double CellRefExpr::eval() const {
	return (*getPtr())->eval();
}

void CellRefExpr::checkCyclic(std::vector<Expression*> prevs) const {
	for (Expression* expP : prevs) {
		if (*getPtr() == expP) {
			throw eval_error("cyclic reference");
		}
	}
	prevs.push_back((Expression*)*getPtr());
	(*getPtr())->checkCyclic(prevs);
}

void CellRefExpr::shift(int dx, int dy) {
	if (!absRow)
		cell.setRow(cell.getRow() + dy);
	if (!absCol)
		cell.setColNum(cell.getColNum() + dx);
}

//Range fuctions ---------------------------------------------------------------
Range::Range(CellRefExpr* top, CellRefExpr* bottom) {
	std::string topCol = top->getCol();
	std::string bottomCol = bottom->getCol();
	int topRow = top->getRow();
	int bottomRow = bottom->getRow();
	std::string minCol = topCol <= bottomCol ? topCol : bottomCol;
	bool minColAbs = topCol <= bottomCol ? top->getAbsCol() : bottom->getAbsCol();
	std::string maxCol = topCol > bottomCol ? topCol : bottomCol;
	bool maxColAbs = topCol > bottomCol ? top->getAbsCol() : bottom->getAbsCol();
	int minRow = topRow <= bottomRow ? topRow : bottomRow;
	bool minRowAbs = topRow <= bottomRow ? top->getAbsRow() : bottom->getAbsRow();
	int maxRow = topRow > bottomRow ? topRow : bottomRow;
	bool maxRowAbs = topRow > bottomRow ? top->getAbsRow() : bottom->getAbsRow();
	topCell = new CellRefExpr(minCol, minRow, top->getSheet(), minColAbs, minRowAbs);
	bottomCell = new CellRefExpr(maxCol, maxRow, top->getSheet(), maxColAbs, maxRowAbs); //assuming both cells are on the same sheet
	delete top;
	delete bottom;
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

Range::iterator Range::begin() const{
	Sheet* sh = topCell->getSheet();
	ExprPointer* beginp = topCell->getPtr();
	ExprPointer* endp = bottomCell->getPtr();
	size_t rangeWidth = (endp - beginp) % (int)(sh->getWidth());
	return iterator(rangeWidth, sh->getWidth(), beginp);
}

Range::iterator Range::end() const{
	Sheet* sh = topCell->getSheet();
	ExprPointer* beginp = topCell->getPtr();
	ExprPointer* endp = bottomCell->getPtr();
	size_t rangeWidth = (endp - beginp) % (int)sh->getWidth();
	return iterator(rangeWidth, sh->getWidth(), endp-rangeWidth+sh->getWidth());
}

//Range iterator fuctions ------------------------------------------------------
Range::iterator& Range::iterator::operator++() {//preinkremens
	if (actCell == nullptr)
		throw std::runtime_error("empty iterator");
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

//FunctionExpr fuctions ------------------------------------------------------
void FunctionExpr::checkCyclic(std::vector<Expression*> prevs) const {
	Range::iterator cell;
	for (cell = range.begin(); cell != range.end(); cell++) {
		for (Expression* exprP : prevs) {
			if (*cell ==  exprP) {
				throw eval_error("cyclic reference");
			}
		}
		prevs.push_back((Expression*)*cell);
		(*cell)->checkCyclic(prevs);
		prevs.pop_back();
	}
}

FunctionExpr* FunctionExpr::newFunctionExpr(FunctionName fname, CellRefExpr* topCell, CellRefExpr* bottomCell){
	switch (fname) {
		case AVG:
			return new AvgFunc(topCell, bottomCell);
		case SUM:
			return new SumFunc(topCell, bottomCell);
		default:
			return nullptr;
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

//Operator fuctions ------------------------------------------------------
Operator& Operator::operator=(const Operator& op){
	if (&op != this) {
		delete lhs;
		lhs = op.lhs->copy();
		delete rhs;
		rhs = op.rhs->copy();
	}
	return *this;
}

Operator* Operator::operandFromToken(Token_type ttype, Expression* lhs, Expression* rhs){
	switch (ttype) {
		case PLUS:
			return new Add(lhs, rhs);
		case MINUS:
			return new Sub(lhs, rhs);
		case STAR:
			return new Mult(lhs, rhs);
		case SLASH:
			return new Div(lhs, rhs);
		default:
			return nullptr;
	}
}
