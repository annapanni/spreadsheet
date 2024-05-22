#include "cell.hpp"

//CellId fuctions --------------------------------------------------------------
CellId::CellId(const std::string& cellstr){
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
