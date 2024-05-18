#include "range.hpp"

//Range fuctions ---------------------------------------------------------------
Range::Range(CellRefExpr* top, CellRefExpr* bottom) {
	std::string topCol = top->getCol();
	std::string bottomCol = bottom->getCol();
	unsigned int topRow = top->getRow();
	unsigned int bottomRow = bottom->getRow();
	std::string minCol = topCol <= bottomCol ? topCol : bottomCol;
	bool minColAbs = topCol <= bottomCol ? top->getAbsCol() : bottom->getAbsCol();
	std::string maxCol = topCol > bottomCol ? topCol : bottomCol;
	bool maxColAbs = topCol > bottomCol ? top->getAbsCol() : bottom->getAbsCol();
	unsigned int minRow = topRow <= bottomRow ? topRow : bottomRow;
	bool minRowAbs = topRow <= bottomRow ? top->getAbsRow() : bottom->getAbsRow();
	unsigned int maxRow = topRow > bottomRow ? topRow : bottomRow;
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
