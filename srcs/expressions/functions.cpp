#include "functions.hpp"

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
