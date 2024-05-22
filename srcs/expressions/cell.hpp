#ifndef CELL_HPP
#define  CELL_HPP


#include <string>
#include <vector>

#include "expression_core.hpp"
#include "../sheet.hpp"
#include "../exceptions.hpp"

///Cellát azonosító sor- és oszlopadat eltárolására szolgáló osztály.
/** oszlop és sorindexeket is 1-től indexelve tárolja, paraméterként is így várja.*/
class CellId {
	unsigned int colNum; ///<oszlop sorszáma 1-től indexelve
	unsigned int row; ///<sorszám 1-től indexelve
public:
	explicit CellId(const std::string& col, unsigned int row) : colNum(Sheet::colNumber(col)), row(row) {}
			///<konstruktor oszlopjelölő betű és sorszám megadásával
	explicit CellId(const std::string&); ///<konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel
	unsigned int getColNum() const {return colNum;} ///<oszlopszám lekérdezése
	unsigned int getRow() const {return row;} ///<sorszám lekérdezése
	void setColNum(unsigned int c) {colNum = c;} ///<oszlopszám beállítása
	void setRow(unsigned int r) {row = r;} ///<sorszám beállítása
	std::string colLetter() const {return Sheet::colLetter(colNum);} ///<oszlopbetű lekérdezése
};

///Cellahivatkozást reprezentáló kifejezés osztály.
/**A hivatkozás egy tábla (Sheet) egy cellájára mutathat oszlop és sor megadásával.
Mind az oszlopa, mind a sora egymástól független lehetnek abszolútak.*/
class CellRefExpr : public Expression {
	CellId cell; ///<cellát azonosító sor- és oszlopadat
	Sheet* refSheet; ///<tábla, amelyre a hivatkozás mutat
	bool absCol; ///<oszlopát tekintve abszolút-e a hivatkozás
	bool absRow; ///<sorát tekintve abszolút-e a hivatkozás
public:
	///konstruktor oszlopjelölő betű és sorszám megadásával
	/**
	@param col - oszlopbetű
	@param row - sorszám
	@param refSheet - tábla, amelyre a hivatkozás mutat
	@param absCol - abszolút hivatkozás-e az oszlop
	@param absRow - abszolút hivatkozás-e a sor
	 */
	explicit CellRefExpr(const std::string& col, unsigned int row, Sheet* refSheet = nullptr, bool absCol=false, bool absRow=false)
		: cell(CellId(col, row)), refSheet(refSheet), absCol(absCol), absRow(absRow) {}

	///konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel
	/**
	@param str - cella jelölője ("[oszlopbetű][sorszám]")
	@param refSheet - tábla, amelyre a hivatkozás mutat
	@param absCol - abszolút hivatkozás-e az oszlop
	@param absRow - abszolút hivatkozás-e a sor
	 */
	explicit CellRefExpr(const std::string& str, Sheet* refSheet = nullptr, bool absCol=false, bool absRow=false)
		: cell(CellId(str)), refSheet(refSheet), absCol(absCol), absRow(absRow) {}
	std::string getCol() const {return cell.colLetter();} ///<oszlopbetű lekérdezése
	unsigned int getRow() const {return cell.getRow();} ///<sorszám lekérdezése
	Sheet* getSheet() const {return refSheet;} ///<hivatkozás által mutatott tábla lekérdezése

	///hivatkozás által mutatott cellára mutató pointer lekérdezése
	ExprPointer* getPtr() const {if (refSheet == nullptr) throw eval_error("uninitialized cell");
		return refSheet->parseCell(cell.getColNum(), cell.getRow());}
	bool getAbsCol() const {return absCol;} ///<oszlop abszolút voltának lekérdezése
	bool getAbsRow() const {return absRow;} ///<sor abszolút voltának lekérdezése
	double eval() const; ///<hivatkozás által mutatott cella kiértékelése
	void checkCyclic(std::vector<Expression*>) const;
	std::string show() const {return (absCol?"$":"") + cell.colLetter() + (absRow?"$":"") + std::to_string(cell.getRow());}
	CellRefExpr* copy() const {return new CellRefExpr(*this);}

	///Eltolja a hivatkozást adott sorral és oszloppal, amennyiben a sor/oszlop nem abszolút
	/**
	@param dx - sor eltolásának mértéke (akár negatív)
	@param dy - oszlop eltolásának mértéke (akár negatív)
	*/
	void shift(int dx, int dy);
	void relocate(Sheet* shp) {refSheet = shp;} ///<a cellahivatkozás célpontját áthelyezi egy másik számolótáblára
};


#endif
